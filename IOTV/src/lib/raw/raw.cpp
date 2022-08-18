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
    case DATA_TYPE::INT_8:
        result = QString::number(*reinterpret_cast<const int8_t*>(_data.data()));
        break;
    case DATA_TYPE::INT_16:
        result = QString::number(qToBigEndian(*reinterpret_cast<const int16_t*>(_data.data())));
        break;
    case DATA_TYPE::INT_32:
        result = QString::number(qToBigEndian(*reinterpret_cast<const int32_t*>(_data.data())));
        break;
    case DATA_TYPE::INT_64:
        //qToBigEndian c int64_t не отрабатывает правильно
        result = QString::number(qToBigEndian(*reinterpret_cast<const qint64*>(_data.data())));
        break;

    case DATA_TYPE::FLOAT_32:
    {
        //        float value = qToBigEndian(*reinterpret_cast<const float*>(_data.data()));
        result = QString::number(*reinterpret_cast<const float*>(_data.data()), 'l', 4);
    }
        break;

    case DATA_TYPE::DOUBLE_64:
    {
        //        double value = qToBigEndian(*reinterpret_cast<const double*>(_data.data()));
        result = QString::number(*reinterpret_cast<const double*>(_data.data()), 'l', 4);
    }
        break;

    case DATA_TYPE::BOOL:
        result = *reinterpret_cast<const bool*>(_data.data()) ? "true" : "false";
        break;
    case DATA_TYPE::STRING:
        result = _data.data();
        break;
    case DATA_TYPE::RAW:
        result = _data.toHex(':');
        break;
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
