#include "raw.h"

Raw::DATA_TYPE Raw::toDataType(iot_uint8_t type)
{
    switch  (type)
    {
    case 0:
        return DATA_TYPE::RAW;
        break;
    case 1:
        return DATA_TYPE::INTEGER_8;
        break;
    case 2:
        return DATA_TYPE::INTEGER_16;
        break;
    case 3:
        return DATA_TYPE::INTEGER_32;
        break;
    case 4:
        //return DATA_TYPE::INTEGER_64;
        break;
    case 5:
        return DATA_TYPE::UNSIGNED_INTEGER_8;
        break;
    case 6:
        return DATA_TYPE::UNSIGNED_INTEGER_16;
        break;
    case 7:
        return DATA_TYPE::UNSIGNED_INTEGER_32;
        break;
    case 8:
        //return DATA_TYPE::UNSIGNED_INTEGER_64;
        break;
    case 9:
        return DATA_TYPE::FLOAT_32;
        break;
    case 10:
        return DATA_TYPE::DOUBLE_32;
        break;
    case 11:
        //return DATA_TYPE::DOUBLE_64;
        break;
    case 12:
        return DATA_TYPE::BOOL_8;
        break;
    case 13:
        return DATA_TYPE::CHAR_PTR;
        break;
    default:
        return DATA_TYPE::RAW;
        break;
    }
}

iot_uint8_t Raw::toUInt8(Raw::DATA_TYPE dataType)
{
    if(dataType == DATA_TYPE::INTEGER_8)
        return 1;
    else if(dataType == DATA_TYPE::INTEGER_16)
        return 2;
    else if(dataType == DATA_TYPE::INTEGER_32)
        return 3;
//    else if(dataType == DATA_TYPE::INTEGER_64)
//        return 4;
    else if(dataType == DATA_TYPE::UNSIGNED_INTEGER_8)
        return 5;
    else if(dataType == DATA_TYPE::UNSIGNED_INTEGER_16)
        return 6;
    else if(dataType == DATA_TYPE::UNSIGNED_INTEGER_32)
        return 7;
//    else if(dataType == DATA_TYPE::UNSIGNED_INTEGER_64)
//        return 8;
    else if(dataType == DATA_TYPE::FLOAT_32)
        return 9;
    else if(dataType == DATA_TYPE::DOUBLE_32)
        return 10;
//    else if(dataType == DATA_TYPE::DOUBLE_64)
//        return 11;
    else if(dataType == DATA_TYPE::BOOL_8)
        return 12;
    else if(dataType == DATA_TYPE::CHAR_PTR)
        return 13;
    else
        return 14;
}
