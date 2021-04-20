#ifndef _MYCONFIGINFO_
#define _MYCONFIGINFO_

#include <datetime16.h>
#include "pinInfo.h"
#include "byteParser.h"
#include "bytePacker.h"



class ConfigInfo
{

private:
    PinInfo *pins;
    uint8_t pinCount;
    PinChangedCallback eventHandler;
    ByteParser parser;
    BytePacker packer;
    uint8_t pinIndexToAdd;
    bool initialized = false;

    // ==== PinId to Index conversion =========
    uint8_t pinIdToIndex(uint8_t pinId);

    // ==== Device info ==========
    uint16_t verTime16, verDate16;      // Mfg date, time
    uint8_t verMinor, verMajor;         // Version major, minor
    const char *deviceName_P;           // Device name as C string in PROGMEM

public:
    /** inicijalizuje objekat sa podacima o datumu proizvodnje, verziji i imenu. Ime je u PROGMEM. */
    ConfigInfo(uint16_t vertime16, uint16_t verdate16, uint8_t vermajor, uint8_t verminor, const char *devicename_P);

    /** Inicijalizuje konfiguraciju za zadati broj parametara. Sme se pozvati samo jednom. */
    void init(uint8_t count);

    /** Dodaje definiciju pina u konfiguraciju. 
     *  Mora se pozivati redom za sve pinove od 0 do count-1.
     *  @@id je pinId, proizvoljna jedinstvena vrednost.
     */
    bool addPin(uint8_t pinId, const char *name, void *valueVar, void *oldValueVar, PinDataType dataType, PinMode pinMode, PinChangedCallback callback);

    /** Dodaje definiciju pina u konfiguraciju. 
     *  Parametar length zadaje maksimalnu veličinu bafera za ByteArray i String. Za ostale tipove se automatski određuje. */
    bool addPin(uint8_t pinId, const char *name, void *valueVar, void *oldValueVar, PinDataType dataType, uint8_t maxLength, PinMode pinMode, PinChangedCallback callback);

    /** Vraća pointer na niz pinova */
    PinInfo *getPins() { return this->pins; };

    /** Vraća broj Config polja */
    uint8_t getPinCount();

    /** Vraća pointer na PinInfo sa zadatim indeksom (0 do 254), ili nullptr ako ne postoji */
    PinInfo *getPinByIndex(uint8_t index);

    /** Vraća pointer na PinInfo sa zadatim pinId (0 do 254), ili nullptr ako ne postoji */
    PinInfo *getPinById(uint8_t id);

    void *getValue(int pinId);
    bool setValue(int pinId, void *value);
    /** Postavlja vrednost pina prostih tipova */
    bool setValue(PinInfo * pi, void *value);
    /** Postavlja vrednost pina tipa ByteArray ili String */
    bool setValue(PinInfo * pi, void *value, uint8_t len);
    void setEventHandler(PinChangedCallback func) { this->eventHandler = func; };

    /** Obrađuje bafer sa komandom (Set, Query, Info ili Ver) i smešta odgovor u dest bafer, dužine destlen */
    uint8_t processCommand(uint8_t *cmd, int8_t cmdlen, uint8_t *dest, uint8_t destlen);

    /** Obrađuje 'SET' poruku u kojoj se nalaze zadate vrednosti pinova.*/
    void processSet(uint8_t *buffer, uint8_t length);

    /** Obrađuje 'QUERY' poruku u message i smešta odgovor u buffer dužine buflen. 
        Vraća dužinu odgovora kao rezultat ili 0 ako ne uspe. */
    uint8_t processQuery(uint8_t *message, uint8_t msglen, uint8_t *buffer, uint8_t buflen);

    /** Obrađuje 'INFO' poruku u message i smešta odgovor u buffer dužine buflen. 
        Vraća dužinu odgovora kao rezultat ili 0 ako ne uspe. */
    uint8_t processInfo(uint8_t *message, uint8_t msglen, uint8_t *buffer, uint8_t buflen);

    /** Obrađuje 'VERSION' poruku u message i smešta odgovor u buffer dužine buflen. 
        Vraća dužinu odgovora kao rezultat ili 0 ako ne uspe. */
    uint8_t processVersion(uint8_t *message, uint8_t msglen, uint8_t *buffer, uint8_t buflen);
};

#endif