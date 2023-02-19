#include "raw.h"

Raw::DATA_TYPE Raw::toDataType(iot_uint8_t type)
{
    switch  (type)
    {
    case 0:
        return DATA_TYPE::INTEGER_8;
        break;
    case 1:
        return DATA_TYPE::INTEGER_16;
        break;
    case 2:
        return DATA_TYPE::INTEGER_32;
        break;
    case 3:
        return DATA_TYPE::INTEGER_64;
        break;
    case 4:
        return DATA_TYPE::FLOAT_32;
        break;
    case 5:
        return DATA_TYPE::DOUBLE_64;
        break;
    case 6:
        return DATA_TYPE::BOOL_8;
        break;
    case 7:
        return DATA_TYPE::STRING;
        break;
    case 8:
        return DATA_TYPE::RAW;
        break;
    default:
        return DATA_TYPE::NONE;
        break;
    }
}

iot_uint8_t Raw::toUInt8(Raw::DATA_TYPE dataType)
{
    if(dataType == DATA_TYPE::INTEGER_8)
        return 0;
    else if(dataType == DATA_TYPE::INTEGER_16)
        return 1;
    else if(dataType == DATA_TYPE::INTEGER_32)
        return 2;
    else if(dataType == DATA_TYPE::INTEGER_64)
        return 3;
    else if(dataType == DATA_TYPE::FLOAT_32)
        return 4;
    else if(dataType == DATA_TYPE::DOUBLE_64)
        return 5;
    else if(dataType == DATA_TYPE::BOOL_8)
        return 6;
    else if(dataType == DATA_TYPE::STRING)
        return 7;
    else if(dataType == DATA_TYPE::RAW)
        return 8;
    else
        return 9; // DATA_TYPE::NONE
}
