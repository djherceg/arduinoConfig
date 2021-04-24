#include <Arduino.h>
#include "configInfo.h"
#include <string.h>
#include <stdlib.h>
#include "pinDataType.h"

ConfigInfo::ConfigInfo(uint8_t pincount, uint16_t vertime16, uint16_t verdate16, uint8_t vermajor, uint8_t verminor, const char *devicename_P)
{
    verTime16 = vertime16;
    verDate16 = verdate16;
    verMajor = vermajor;
    verMinor = verminor;
    deviceName_P = devicename_P;

    pinCount = pincount;
    pins = (PinInfo *)malloc(sizeof(PinInfo) * pinCount);
    pinIndexToAdd = 0;
    initialized = true;
};

// void ConfigInfo::init(uint8_t count)
// {
//     if (initialized)
//     {
//         Serial.println(F("ConfigInfo::init(uint8_t) can only be invoked once! Halting."));
//         while (true)
//         {
//         }
//     }

//     pinCount = count;
//     pins = (PinInfo *)malloc(sizeof(PinInfo) * count);
//     pinIndexToAdd = 0;
//     initialized = true;
//     Serial.print(F("ConfigInfo::init FieldCount je: "));
//     Serial.println(pinCount, DEC);
// }



bool ConfigInfo::addPin(uint8_t pinId, const char *name, void *valueVar, void *oldValueVar, PinDataType dataType, uint8_t maxLength, PinMode pinMode, PinChangedCallback callback)
{
    if ((pinIndexToAdd <= this->pinCount) && (pinId < PINID_INVALID))
    { // check for memory overruns
        this->pins[pinIndexToAdd].id = pinId;
        this->pins[pinIndexToAdd].name = name;
        this->pins[pinIndexToAdd].value = valueVar;
        this->pins[pinIndexToAdd].type = dataType;
        this->pins[pinIndexToAdd].mode = pinMode;
        this->pins[pinIndexToAdd].maxLen = maxLength;
        this->pins[pinIndexToAdd].valueOld = oldValueVar;
        this->pins[pinIndexToAdd].callback = callback;

        pinIndexToAdd++;
        return true;
    }
    else
        return false;
}

bool ConfigInfo::addPin(uint8_t pinId, const char *name, void *valueVar, void *oldValueVar, PinDataType dataType, PinMode pinMode, PinChangedCallback callback)
{
    if ((dataType == PinDataType::pdtUnknown) || (dataType == PinDataType::pdtByteArray))
    {
        return false; // String i ByteArray moraju da zadaju konkretnu maksimalnu dužinu bafera pomoću drugog metoda
    }
    if (dataType == PinDataType::pdtString)
    {
        uint8_t len = strlen((char *)valueVar);
        return addPin(pinId, name, valueVar, oldValueVar, dataType, len, pinMode, callback);
    }
    else
    {
        uint8_t len = pdtLen(dataType);
        if (len != 0)
            return addPin(pinId, name, valueVar, oldValueVar, dataType, len, pinMode, callback);
        else
            return false;
    }
}

void *ConfigInfo::getValue(int pinId)
{
    uint8_t index = pinIdToIndex(pinId);
    if (index == PINID_INVALID)
        return nullptr;
    else
    {
        return pins[index].getValue();
    }
}



bool ConfigInfo::setValue(PinInfo * pi, void *value)
{
    if (pi == nullptr)
        return false;
    else
    {
        pi->setValue(value);
        if (eventHandler != nullptr)
            eventHandler(pi);
        if (pi->callback != nullptr)
            pi->callback(pi);
        return true;
    }

    return false;
}

bool ConfigInfo::setValue(PinInfo * pi, void *value, uint8_t len)
{
    if (pi == nullptr)
        return false;
    else
    {
        pi->setValue(value, len);
        if (eventHandler != nullptr)
            eventHandler(pi);
        if (pi->callback != nullptr)
            pi->callback(pi);
        return true;
    }

    return false;
}

PinInfo *ConfigInfo::getPinById(uint8_t pinId)
{
    uint8_t index = pinIdToIndex(pinId);
    if (index == PINID_INVALID)
        return nullptr;
    else
        return &pins[index];
}

PinInfo *ConfigInfo::getPinByIndex(uint8_t index)
{
    if (index < pinCount)
        return &pins[index];
    else
        return nullptr;
}

uint8_t ConfigInfo::pinIdToIndex(uint8_t pinId)
{
    for (int i = 0; i < pinCount; i++)
    {
        if (pins[i].id == pinId)
            return i;
    }
    return PINID_INVALID;
}

int ConfigInfo::processCommand(uint8_t *cmd, int8_t cmdlen, uint8_t *dest, uint8_t destlen)
{
    switch ((char)cmd[0])
    {
    case 'I': // INFO
        // Serial.println(F("ConfigMan::processCommand.."));
        return this->processInfo(cmd + 1, cmdlen - 1, dest, destlen);

    case 'S': // SET
        // Serial.println(F("ConfigMan::processCommand..S"));
        this->processSet(cmd + 1, cmdlen - 1);
        return 0;

    case 'Q': // QUERY
        return this->processQuery(cmd + 1, cmdlen - 1, dest, destlen);

    case 'V': // VER
        return this->processVersion(cmd + 1, cmdlen - 1, dest, destlen);
    }

    return 0;
}

int ConfigInfo::processInfo(uint8_t *message, uint8_t msglen, uint8_t *buffer, uint8_t buflen)
{
    /* Ovaj metod podržava poziv bez parametara ili sa parametrima
     * Bez parametara vraća broj config polja kao bajt.
     * Parametri su pojedinačni indeksi config polja od 0 do 254, kao bajtovi.
     * Tada je rezultat niz info paketa oblika
     *  - id (byte)
     *  - type (byte)
     *  - name (C string)
     * 
     * Metod vraća broj bajtova upisanih u izlazni bafer. 
     * Ako vrati 0 to znači da upis nije uspeo.
     */

    //Serial.println(F("ConfigInfo::processInfo.."));

    parser.init(message, msglen); // parser tumači dolaznu poruku
    packer.init(buffer, buflen);  // packer upisuje vrednosti parametara u izlazni bafer

    // prvo upisujemo 'I' kao odgovor na komandu 'I' bez parametara
    if (!packer.write('I'))
        return 0;

    // prva varijanta, kada se traži broj polja u ConfigInfo strukturi
    if (msglen == 0)
    {
        if (packer.write(pinCount)) // upiši broj polja u izlazni bafer
        {
            return packer.getPos(); // vrati dužinu izlaznog bafera - tolika je poruka za odgovor
        }
        else
            return 0;
    }

    // druga varijanta, kada se traže metapodaci o pinovima sa navedenim indeksima
    uint8_t b;
    while (!parser.isEOB()) // čitaj dolaznu poruku sve dok ima bajtova
    {
        parser.getNextByte(b); // pročitaj indeks traženog polja iz dolazne poruke
        //Serial.print(F("..parser.getNextByte returned: "));
        //Serial.println(b);
        // vrati metapodatke o željenom config polju
        PinInfo *cfi = getPinByIndex(b);

        int l = strlen_P(cfi->name); // nađi dužinu imena polja
        //Serial.print(F("..strlen_P(cfi->getName()) returned: "));
        //Serial.println(l);
        if ((cfi != nullptr) && (packer.isfreen(3 + l))) // da li u baferu ima mesta za info paket (id+tip+string+0x0)
        {
            //Serial.println(cfi->getName());
            packer.write(cfi->id);         // ID polja
            packer.write(cfi->type);       // tip polja
            packer.putString_P(cfi->name); // naziv polja
            //packer.putPString(cfi->getName()); // naziv polja
            //packer.getPos(); // dodaj dužinu generisanog odgovora u izlaznom baferu TODO - verovatno nepotrebno
            parser.moveNext();
        }
        else
            return 0; // upis rezultata u izlazni bafer nije uspeo, vrati 0
    }

    return packer.getPos(); // vrati dužinu celokupnog odgovora
}

int ConfigInfo::processQuery(uint8_t *message, uint8_t msglen, uint8_t *buffer, uint8_t buflen)
{
    PinInfo *fld;
    uint8_t id;

    packer.init(buffer, buflen);

    for (uint8_t i = 0; i < msglen; i++)
    {
        id = message[i];
        fld = getPinById(id);
        if (fld != nullptr)
        {
            switch (fld->type)
            {
            case PinDataType::pdtBool:
                if (packer.putBool(id, (bool *)fld->getValue()))
                {
                    //Serial.println(F("ok"));
                }
                else
                {
                    //Serial.println(F("failed"));
                }

                break;

            case PinDataType::pdtByte:
                //Serial.println(F("byte"));
                packer.putByte(id, *(uint8_t *)fld->getValue());
                break;

            case PinDataType::pdtSByte:
                //Serial.println(F("sbyte"));
                break;

            case PinDataType::pdtInt16:
                //Serial.println(F("int16"));
                packer.putInt16(id, *(int16_t *)fld->getValue());
                break;

            case PinDataType::pdtUInt16:
                //Serial.println(F("uint16"));
                packer.putUInt16(id, *(uint16_t *)fld->getValue());
                break;

            case PinDataType::pdtInt32:
                //Serial.println(F("int32"));
                packer.putInt32(id, *(int32_t *)fld->getValue());
                break;

            case PinDataType::pdtUInt32:
                //Serial.println(F("uint32"));
                packer.putUInt32(id, *(uint32_t *)fld->getValue());
                break;

            case PinDataType::pdtFloat:
                //Serial.println(F("float"));
                packer.putFloat(id, *(float *)fld->getValue());
                break;

            case PinDataType::pdtTime16:
                break;

            case PinDataType::pdtDate16:
                break;

            case PinDataType::pdtByteArray:
                break;

            case PinDataType::pdtString:
                break;

            default:
                // ignoriši sve ostale tipove uključujući i pdtUnknown
                break;
            }
            //Serial.print(F("Upis u izlazni bafer uspeo. Pos: "));
            //Serial.println(packer.getPos());
        }
        else
        {
            //Serial.print(F("PinInfo sa ID: "));
            //Serial.print(message[i]);
            //Serial.println(F(" ne postoji."));
        }
    }
    return packer.getPos(); // vrati dužinu odgovora
}

int ConfigInfo::processSet(uint8_t *buffer, uint8_t length)
{
    uint8_t pinId;      // id pina u Set baferu
    uint8_t lenOut;     // dužina nove vrednosti u Set baferu
    PinDataType pdtOut; // datatype nove vrednosti u Set baferu
    //void *newValue;     // pointer na novu vrednost u Set baferu

    parser.init(buffer, length);
    parser.moveFirst();
    Serial.println("parser.CheckAllValues()");
    if (parser.checkAllValues())
    {
        //Serial.println("while (!parser.isEOB())");
        while (!parser.isEOB())
        {
            //Serial.println("parser.checkNextValue()");
            parser.checkNextValue(pinId, pdtOut, lenOut);
            Serial.print("Paket za pinId: ");
            Serial.println(pinId);

            PinInfo *cfi = getPinById(pinId);
            if ((cfi != nullptr) && (cfi->type == pdtOut) && (cfi->mode == PinMode::pmOutput))
            {
                // proveri tip podataka i da li je pin izlazni
                // dodeli vrednost pinu
                // TODO: pdtLen(pdt) proveriti kako se ponaša za ByteArray i String!
                parser.getNextValue(cfi->getValue(), cfi->getValueOld(), pdtLen(pdtOut), pinId, pdtOut, lenOut);

                Serial.print("Dodeljena vrednost pinu: ");
                Serial.print(pinId);
                Serial.print(" pdt: ");
                Serial.print(pdtOut);
                Serial.print(" len: ");
                Serial.print(lenOut);
                Serial.println();

                // pozovi Event handler
                if (eventHandler != nullptr){
                    Serial.println(F("Invoking eventHandler (ConfigInfo::processSet)"));
                    eventHandler(getPinById(pinId));
                }
                if (cfi->callback != nullptr)
                    cfi->callback(cfi);
            }
            parser.moveNext();
        }
        return parser.getPos();     // vrati poziciju u baferu parsera nakon svih komandi kao indikaciju uspeha
    }
    else
    {
        //Serial.println(F("Prekidam ConfigInfo::processMessage"));
        return 0;
    }
}

int ConfigInfo::processVersion(uint8_t *message, uint8_t msglen, uint8_t *buffer, uint8_t buflen)
{
    /* Vraća verziju softvera
     * Bez parametara vraća broj config polja kao bajt.
     * Parametri su pojedinačni indeksi config polja od 0 do 254, kao bajtovi.
     * Tada je rezultat niz info paketa oblika
     *  - id (byte)
     *  - type (byte)
     *  - name (C string)
     */

    parser.init(message, msglen);
    packer.init(buffer, buflen);

    // prva varijanta, kada se traži broj polja u ConfigInfo strukturi
    if (msglen == 0)
    {
        if (packer.write(pinCount))
        {
            return packer.getPos();
        }
        else
            return 0;
    }

    uint8_t b;
    while (!parser.isEOB())
    {
        parser.getNextByte(b);
        // vrati metapodatke o željenom config polju
        PinInfo *cfi = getPinByIndex(b);

        int l = strlen(cfi->name);
        if ((cfi != nullptr) && (packer.isfreen(3 + l))) // da li u baferu ima mesta za info paket (id+tip+string+0x0)
        {
            //Serial.println(cfi->getName());
            packer.write(b);               // ID polja
            packer.write(cfi->type);       // tip polja
            packer.putString_P(cfi->name); // naziv polja
            //packer.getPos();                    // dodaj dužinu generisanog odgovora u izlaznom baferu
            parser.moveNext();
        }
        else
            return 0; // upis rezultata u izlazni bafer nije uspeo, vrati 0
    }

    return packer.getPos();
}
