#ifndef _PININFO_
#define _PININFO_

#include "stdint.h"
#include "pindatatype.h"
#include <string.h>
#include <avr/pgmspace.h>


class PinInfo;
typedef void (*PinChangedCallback)(PinInfo *);

class PinInfo
{
public:
    uint8_t id;
    void *value;
    void *valueOld;
    const char *name;
    PinDataType type;
    PinMode mode;
    uint8_t maxLen;
    PinChangedCallback callback;

    //PinInfo(int id, const char *name, PinDataType dataType, void *value, PinMode pinMode);
    PinInfo(int id, const char *name, PinDataType dataType, void *value, void *valueOld, PinMode pinMode) : PinInfo(id, name, dataType, value, valueOld, pinMode, nullptr) {}
    PinInfo(int id, const char *name, PinDataType dataType, void *value, void *valueOld, PinMode pinMode, PinChangedCallback callback);
    PinInfo(){};

    //uint8_t getLength() { return this->length; }

    /** Vraća pointer na vrednost pina */
    void *getValue() { return this->value; };

    /** Vraća pointer na staru vrednost pina */
    void *getValueOld() { return this->valueOld; };

    void setOldValue(void *oldValue){};
    
    /** Dodeljuje vrednosti prostih tipova */
    void setValue(void *val);
    
    /** Dodeljuje ByteArray i String vrednosti zadate dužine */
    void setValue(void *val, uint8_t len);      

    /** Zadaje callback funkciju koja se poziva kada se vrednost pina izmeni */
    //void setPinChangeCallback(PinChangedCallback func) { this->callback = func; };

    /** Zadaje dužinu promenljive u bajtovima */
    //void setLength(uint8_t len) { this->maxLen = len; };
};

#endif