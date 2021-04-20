#include "pinInfo.h"
#include <Arduino.h>

PinInfo::PinInfo(int pinId, const char *name, PinDataType dataType, void *value, void *valueOld, PinMode pinMode, PinChangedCallback callback)
{
    Serial.print(F("pin ID: "));
    Serial.println(pinId);
    this->id = pinId;
    this->name = name;
    this->type = dataType;
    if (dataType == pdtString)
        this->maxLen = strlen((char *)value); // ignored for anything except String and ByteArray
    this->value = value;
    this->valueOld = valueOld;
    this->mode = pinMode;
    this->callback = callback;
};

void PinInfo::setValue(void *val)
{
    if ((this->type == PinDataType::pdtString) && (this->type == PinDataType::pdtByteArray))
        return;

    // copy the old value if valueOld is defined
    if (this->valueOld != nullptr)
        memcpy(this->valueOld, this->value, pdtLen(this->type));

    // copy the actual value
    memcpy(this->value, val, pdtLen(this->type));
};

void PinInfo::setValue(void *val, uint8_t len)
{
    if ((this->type != PinDataType::pdtString) && (this->type != PinDataType::pdtByteArray))
        return;

    // copy the old value if valueOld is defined
    if (this->valueOld != nullptr)
    {
        // copy the array up to maxLen bytes
    }

    // copy the actual value
    Serial.println(F("memcpy for arrays executing"));
    int m = min(len, this->maxLen);
    memcpy(this->value, val, m);
    if (this->type == PinDataType::pdtString)
        ((char *)this->value)[m] = 0;           // null-terminate strings
};