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

uint16_t Raw::size() const
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
