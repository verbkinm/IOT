#include "raw.h"

Raw::Raw() : _type(DATA_TYPE::NONE)
{

}

Raw::Raw(DATA_TYPE type) : _type(type)
{

}

Raw::Raw(DATA_TYPE type, const QByteArray &data) : _type(type), _data(data)
{

}

uint16_t Raw::size() const
{
    return _data.size();
}

const QByteArray &Raw::data() const
{
    return _data;
}

void Raw::setData(const QByteArray &newData)
{
    _data = newData;
}

QString Raw::strData() const
{
    QString result;

    switch (_type)
    {
    //UNSIGNED
    case DATA_TYPE::UNSIGNED_INTEGER_8:
        result = QString::number(*reinterpret_cast<const uint8_t*>(_data.data()));
        break;
    case DATA_TYPE::UNSIGNED_INTEGER_16:
        result = QString::number(*reinterpret_cast<const uint16_t*>(_data.data()));
        break;
    case DATA_TYPE::UNSIGNED_INTEGER_32:
        result = QString::number(*reinterpret_cast<const uint32_t*>(_data.data()));
        break;
    case DATA_TYPE::UNSIGNED_INTEGER_64:
        result = QString::number(*reinterpret_cast<const uint64_t*>(_data.data()));
        break;
    //SIGNED
    case DATA_TYPE::INTEGER_8:
        result = QString::number(*reinterpret_cast<const int8_t*>(_data.data()));
        break;
    case DATA_TYPE::INTEGER_16:
        result = QString::number(*reinterpret_cast<const int16_t*>(_data.data()));
        break;
    case DATA_TYPE::INTEGER_32:
        result = QString::number(*reinterpret_cast<const int32_t*>(_data.data()));
        break;
    case DATA_TYPE::INTEGER_64:
        result = QString::number(*reinterpret_cast<const int64_t*>(_data.data()));
        break;
    //REAL
    case DATA_TYPE::FLOAT_32:
        result = QString::number(*reinterpret_cast<const float*>(_data.data()), 'l', 4);
        break;
    case DATA_TYPE::DOUBLE_32:
    case DATA_TYPE::DOUBLE_64:
        result = QString::number(*reinterpret_cast<const double*>(_data.data()), 'l', 4);
        break;
    //BOOL
    case DATA_TYPE::BOOL_8:
        result = *reinterpret_cast<const bool*>(_data.data()) ? "true" : "false";
        break;
    //STRING
    case DATA_TYPE::STRING:
        result = _data.data();
        break;
    //RAW
    case DATA_TYPE::RAW:
        result = _data.toHex(':');
        break;
    //NONE
    default:
        result = "";
        break;
    }

    return result;
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
