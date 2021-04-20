#include "byteParser.h"
#include <string.h>
#include <Arduino.h>

/**
 * Povezuje parser sa baferom zadate dužine i postavlja poziciju na prvi bajt bafera.
 */
void ByteParser::init(uint8_t *buffer, uint8_t length)
{
    buf = buffer;
    len = length;
    moveFirst();
}

void ByteParser::moveFirst()
{
    pos = 0;
    posSkip = 0;
}

bool ByteParser::moveNext()
{
    if ((posSkip > 0) && (pos + posSkip <= len))
    {
        pos += posSkip;
        return true;
    }
    else
        return false;
}

uint8_t ByteParser::getPos()
{
    return pos;
}

uint8_t ByteParser::getLen()
{
    return len;
}

bool ByteParser::isEOB()
{
    return (pos == len);
}

bool ByteParser::isnext()
{
    return pos < len;
}

bool ByteParser::isnextn(int n)
{
    return (pos + n) <= len;
}

uint8_t ByteParser::read()
{
    if (pos < len)
        return buf[pos++];
    else
        return 0;
}

/*
bool ByteParser::canPeek(uint8_t n)
{
    return pos + n < len;
}
*/

uint8_t ByteParser::peek(uint8_t n)
{
    if (pos + n < len)
        return buf[pos + n];
    else
        return 0;
}

/**
 * Dozvoljeni su svi PinDataType, osim pdtUnknown.
 * Format paketa: [pinID][dataType]{dataLength}[data]
 *  pinID (uint8_t)                  - ID pina
 *  dataType (uint8_t)               - tip podataka         
 *  dataLength (uint8_t)             - dužina vrednosti (samo za bytearray i string, ostali su konstantni)
 *  data (uint8_t[dataLength])       - vrednost
 */
bool ByteParser::getNextValue(void *var, uint8_t maxlen, uint8_t &pinIdOut, PinDataType &pdtOut, uint8_t &lenOut)
{
    if (isnextn(2))
    {
        // pinID
        pinIdOut = peek(0);

        // tip podatka
        pdtOut = itoPdt(peek(1));

        switch (pdtOut)
        {
        case PinDataType::pdtBool:
        case PinDataType::pdtByte:
        case PinDataType::pdtSByte:
            lenOut = 1;
            break;

        case PinDataType::pdtInt16:
        case PinDataType::pdtUInt16:
        case PinDataType::pdtTime16:
        case PinDataType::pdtDate16:
            lenOut = 2;
            break;

        case PinDataType::pdtUInt32:
        case PinDataType::pdtInt32:
        case PinDataType::pdtFloat:
            lenOut = 4;
            break;

        case PinDataType::pdtByteArray:
            lenOut = peek(2);
            // nizove promenljive dužine vraćamo iz ovog bloka:
            if (isnextn(2 + lenOut) && (lenOut <= maxlen))
            {
                memcpy(var, &buf[pos + 3], lenOut);
                posSkip = lenOut + 3; // ukupno preskoči 3 + lenOut bajtova
                return true;
            }

            // Obradi grešku
            pinIdOut = PINID_INVALID;
            pdtOut = PinDataType::pdtUnknown;
            lenOut = 0;
            posSkip = 0;
            return false;
            break;

        case PinDataType::pdtString:
            lenOut = peek(2);
            // Pascal-like stringove promenljive dužine vraćamo iz ovog bloka:
            if (isnextn(2 + lenOut) && (lenOut + 1 <= maxlen))
            {
                memcpy(var, &buf[pos + 3], lenOut);
                posSkip = lenOut + 3; // ukupno prekoči 3 + lenOut bajtova
                lenOut++;             // signaliziraj pozivaocu da je na kraj stringa dodat NUL karakter
                return true;
            }

            // Obradi grešku
            pinIdOut = PINID_INVALID;
            pdtOut = PinDataType::pdtUnknown;
            lenOut = 0;
            posSkip = 0;
            return false;
            break;

        default:
            pinIdOut = PINID_INVALID;
            pdtOut = PinDataType::pdtUnknown;
            lenOut = 0;
            posSkip = 0;
            return false; // tip nije prepoznat, vrati false
        }

        if (isnextn(1 + lenOut) && (lenOut <= maxlen)) // da li ulazni bafer ima dovoljno bajtova i da li izlazni bafer ima dovoljan kapacitet?
        {
            memcpy(var, &buf[pos + 2], lenOut);
            posSkip = lenOut + 2;
            return true;
        }
    }

    pinIdOut = PINID_INVALID;
    pdtOut = PinDataType::pdtUnknown;
    lenOut = 0;
    posSkip = 0;
    return false;
}

//
// #### OVO SE RAZVIJA ###############################################################################################################
//
bool ByteParser::getNextValue(uint8_t &pinIdOut, PinDataType &pdtOut, uint8_t &lenOut, void *valueBuffer)
{
    if (isnextn(2))
    {
        // pinKey
        pinIdOut = peek(0);

        // tip podatka
        pdtOut = itoPdt(peek(1));

        switch (pdtOut)
        {
        case PinDataType::pdtBool:
        case PinDataType::pdtByte:
        case PinDataType::pdtSByte:
        case PinDataType::pdtInt16:
        case PinDataType::pdtUInt16:
        case PinDataType::pdtTime16:
        case PinDataType::pdtDate16:
        case PinDataType::pdtUInt32:
        case PinDataType::pdtInt32:
        case PinDataType::pdtFloat:
            lenOut = pdtLen(pdtOut);
            if (isnextn(1 + lenOut)) // da li ulazni bafer ima dovoljno bajtova?
            {
                valueBuffer = &buf[pos + 2];
                posSkip = lenOut + 2;
                return true;
            }
            break;

        case PinDataType::pdtByteArray:
        case PinDataType::pdtString:
            lenOut = peek(2);
            // nizove promenljive dužine vraćamo iz ovog bloka:
            if (isnextn(2 + lenOut))
            {
                valueBuffer = &buf[pos + 3];
                posSkip = lenOut + 3; // ukupno preskoči 3 + lenOut bajtova
                return true;
            }
            break;

        case PinDataType::pdtUnknown:
            // nepoznati tip, vrati false
            return false;
        }
    }

    pinIdOut = PINID_INVALID;
    pdtOut = PinDataType::pdtUnknown;
    lenOut = 0;
    posSkip = 0;
    return false;
}

/**
 * Dozvoljeni su svi PinDataType, osim pdtUnknown.
 * Format paketa: [pinKey][dataType]{dataLength}[data]
 *  pinId (uint8_t)                  - ID pina
 *  dataType (uint8_t)               - tip podataka         
 *  dataLength (uint8_t)             - dužina vrednosti (samo za bytearray i string, ostali su konstantni)
 *  data (uint8_t[dataLength])       - vrednost
 */
bool ByteParser::getNextValue(void *var, void *oldVar, uint8_t maxlen, uint8_t &pinIdOut, PinDataType &pdtOut, uint8_t &lenOut)
{
    if (isnextn(2))
    {
        // pinKey
        pinIdOut = peek(0);

        // tip podatka
        pdtOut = itoPdt(peek(1));

        switch (pdtOut)
        {
        case PinDataType::pdtBool:
        case PinDataType::pdtByte:
        case PinDataType::pdtSByte:
        case PinDataType::pdtInt16:
        case PinDataType::pdtUInt16:
        case PinDataType::pdtTime16:
        case PinDataType::pdtDate16:
        case PinDataType::pdtUInt32:
        case PinDataType::pdtInt32:
        case PinDataType::pdtFloat:
            lenOut = pdtLen(pdtOut);
            break;

        case PinDataType::pdtByteArray:
            lenOut = peek(2);
            // nizove promenljive dužine vraćamo iz ovog bloka:
            if (isnextn(2 + lenOut) && (lenOut <= maxlen))
            {
                memcpy(oldVar, var, lenOut); // TODO: Proveriti - ovo skoro sigurno ne valja zbog lenOut
                memcpy(var, &buf[pos + 3], lenOut);
                posSkip = lenOut + 3; // ukupno preskoči 3 + lenOut bajtova
                return true;
            }

            pinIdOut = PINID_INVALID;
            pdtOut = PinDataType::pdtUnknown;
            lenOut = 0;
            posSkip = 0;
            return false;
            break;

        case PinDataType::pdtString:
            lenOut = peek(2);
            // Pascal-like stringove promenljive dužine vraćamo iz ovog bloka:
            if (isnextn(2 + lenOut) && (lenOut + 1 <= maxlen))
            {
                memcpy(oldVar, var, lenOut); // TODO: Proveriti - ovo skoro sigurno ne valja zbog lenOut
                memcpy(var, &buf[pos + 3], lenOut);
                posSkip = lenOut + 3; // ukupno prekoči 3 + lenOut bajtova
                lenOut++;             // signaliziraj pozivaocu da je na kraj stringa dodat NUL karakter
                return true;
            }

            pinIdOut = PINID_INVALID;
            pdtOut = PinDataType::pdtUnknown;
            lenOut = 0;
            posSkip = 0;
            return false;
            break;

        default:
            pinIdOut = PINID_INVALID;
            pdtOut = PinDataType::pdtUnknown;
            lenOut = 0;
            posSkip = 0;
            return false; // tip nije prepoznat, vrati false
        }

        if (isnextn(1 + lenOut) && (lenOut <= maxlen)) // da li ulazni bafer ima dovoljno bajtova i da li izlazni bafer ima dovoljan kapacitet?
        {
            memcpy(oldVar, var, lenOut);
            memcpy(var, &buf[pos + 2], lenOut);
            posSkip = lenOut + 2;
            return true;
        }
    }

    pinIdOut = PINID_INVALID;
    pdtOut = PinDataType::pdtUnknown;
    lenOut = 0;
    posSkip = 0;
    return false;
}

/**
 * Dozvoljeni su svi PinDataType, osim pdtUnknown.
 * Kada završi, resetuje pos = 0.
 */
bool ByteParser::checkAllValues()
{
    moveFirst();
    uint8_t p, l;
    PinDataType pdt;
    while (pos < len)
    {
        if (!checkNextValue(p, pdt, l))
            return false;
        else
            moveNext();
    }
    moveFirst();
    return true;
}

/**
 * Dozvoljeni su svi PinDataType, osim pdtUnknown.
 * Ako je podatak validan, ažurira posSkip. Ako nije, posSkip = 0.
 */
bool ByteParser::checkNextValue(uint8_t &pinIdOut, PinDataType &pdtOut, uint8_t &lenOut)
{
    if (isnextn(2))
    {
        // pinKey
        pinIdOut = peek(0);

        // tip podatka
        pdtOut = itoPdt(peek(1));

        switch (pdtOut)
        {
        case PinDataType::pdtBool:
        case PinDataType::pdtByte:
        case PinDataType::pdtSByte:
        case PinDataType::pdtInt16:
        case PinDataType::pdtUInt16:
        case PinDataType::pdtTime16:
        case PinDataType::pdtDate16:
        case PinDataType::pdtUInt32:
        case PinDataType::pdtInt32:
        case PinDataType::pdtFloat:
            lenOut = pdtLen(pdtOut);
            break;

        case PinDataType::pdtByteArray:
            lenOut = peek(2);
            // nizove promenljive dužine vraćamo iz ovog bloka:
            if (isnextn(2 + lenOut))
            {
                //memcpy(var, &buf[pos], lenOut);
                posSkip = lenOut + 3; // ukupno preskoči 3 + lenOut bajtova
                return true;
            }
            break;

        case PinDataType::pdtString:
            lenOut = peek(2);
            // Pascal-like stringove promenljive dužine vraćamo iz ovog bloka:
            if (isnextn(2 + lenOut))
            {
                //memcpy(var, &buf[pos], lenOut);
                posSkip = lenOut + 3; // ukupno prekoči 3 + lenOut bajtova
                lenOut++;             // signaliziraj pozivaocu da se na kraj stringa dodaje NUL karakter
                //((char*)var)[lenOut] = 0;       // null-terminate!
                return true;
            }
            break;

        default:
            pinIdOut = PINID_INVALID;
            pdtOut = PinDataType::pdtUnknown;
            lenOut = 0;
            posSkip = 0;
            return false; // tip nije prepoznat, vrati false
        }

        if (isnextn(1 + lenOut)) // da li ulazni bafer ima dovoljno bajtova?
        {
            //memcpy(var, &buf[pos], lenOut);
            posSkip = lenOut + 2; // ukupno preskoči lenOut + 2 bajta
            return true;
        }
    }

    pinIdOut = PINID_INVALID;
    pdtOut = PinDataType::pdtUnknown;
    lenOut = 0;
    posSkip = 0;
    return false;
}

bool ByteParser::getNextByte(uint8_t &byteOut)
{
    if (isnextn(1))
    {
        byteOut = buf[pos];
        posSkip = 1;
        return true;
    }
    else
        return false;
}