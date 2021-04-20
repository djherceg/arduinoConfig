#include "pindatatype.h"

PinDataType itoPdt(int i)
{
    switch (i)
    {
    case 0:
        return PinDataType::pdtBool;
        break;
    case 1:
        return PinDataType::pdtSByte;
        break;
    case 2:
        return PinDataType::pdtByte;
        break;
    case 3:
        return PinDataType::pdtInt16;
        break;
    case 4:
        return PinDataType::pdtUInt16;
        break;
    case 5:
        return PinDataType::pdtInt32;
        break;
    case 6:
        return PinDataType::pdtUInt32;
        break;
    case 9:
        return PinDataType::pdtByteArray;
        break;
    case 10:
        return PinDataType::pdtString;
        break;
    case 11:
        return PinDataType::pdtFloat;
        break;
    case 12:
        return PinDataType::pdtTime16;
        break;
    case 13:
        return PinDataType::pdtDate16;
        break;
    default:
        return PinDataType::pdtUnknown;
        break;
    }
}

uint8_t pdtLen(PinDataType pdt)
{
    switch (pdt)
    {
    case PinDataType::pdtBool:
    case PinDataType::pdtSByte:
    case PinDataType::pdtByte:
        return 1;

    case PinDataType::pdtInt16:
    case PinDataType::pdtUInt16:
        return 2;

    case PinDataType::pdtInt32:
    case PinDataType::pdtUInt32:
    case PinDataType::pdtFloat:
    case PinDataType::pdtTime16:
    case PinDataType::pdtDate16:
        return 4;

    case PinDataType::pdtByteArray:
    case PinDataType::pdtString:
    case PinDataType::pdtUnknown:
    default:
        return 0;
    }
}