#include "raw.h"

Raw::Raw() : _type(DATA_TYPE::NONE)
{

}

Raw::Raw(DATA_TYPE type) : _type(type)
{

}

Raw::Raw(DATA_TYPE type, const std::vector<uint8_t> &data) : _type(type), _data(data)
{

}

Raw::DATA_TYPE Raw::toDataType(uint8_t type)
{
    switch(type)
    {
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
        return DATA_TYPE::INTEGER_64;
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
        return DATA_TYPE::UNSIGNED_INTEGER_64;
        break;
    case 9:
        return DATA_TYPE::FLOAT_32;
        break;
    case 10:
        return DATA_TYPE::DOUBLE_32;
        break;
    case 11:
        return DATA_TYPE::DOUBLE_64;
        break;
    case 12:
        return DATA_TYPE::BOOL_8;
        break;
    case 13:
        return DATA_TYPE::STRING;
        break;
    default:
        return DATA_TYPE::RAW;
        break;
    }
}

unsigned short Raw::size() const
{
    return _data.size();
}

const std::vector<uint8_t> &Raw::data() const
{
    return _data;
}

void Raw::setData(const std::vector<uint8_t> &newData)
{
    _data = newData;
}

void Raw::push_back(uint8_t byte)
{
    _data.push_back(byte);
}

void Raw::clear()
{
    _data.clear();
}

Raw::DATA_TYPE Raw::type() const
{
    return _type;
}

void Raw::setType(DATA_TYPE newType)
{
    _type = newType;
}

bool operator==(const Raw &lhs, const Raw &rhs)
{
    return (lhs.type() == rhs.type() && lhs.data() == rhs.data());
}
