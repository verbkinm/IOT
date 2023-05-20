#include "raw.h"

Raw::Raw() : _type(DATA_TYPE::NONE)
{

}

Raw::Raw(int8_t data) :
    _type(DATA_TYPE::INT_8),
    _data{1, data}
{

}

Raw::Raw(int16_t data) :
    _type(DATA_TYPE::INT_16)
{
    if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
        data = qToLittleEndian(data);

    char *ptr = reinterpret_cast<char*>(&data);
    for (uint i = 0; i < sizeof(data); i++)
        _data.push_back(ptr[i]);
}

Raw::Raw(int32_t data) :
    _type(DATA_TYPE::INT_32)
{
    if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
        data = qToLittleEndian(data);

    char *ptr = reinterpret_cast<char*>(&data);
    for (uint i = 0; i < sizeof(data); i++)
        _data.push_back(ptr[i]);
}

Raw::Raw(int64_t data) :
    _type(DATA_TYPE::INT_64)
{
    if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
        data = qToLittleEndian(data);

    char *ptr = reinterpret_cast<char*>(&data);
    for (uint i = 0; i < sizeof(data); i++)
        _data.push_back(ptr[i]);
}

Raw::Raw(float data) :
    _type(DATA_TYPE::FLOAT_32)
{
    char *ptr = reinterpret_cast<char*>(&data);
    for (uint i = 0; i < sizeof(data); i++)
        _data.push_back(ptr[i]);
}

Raw::Raw(double data) :
    _type(DATA_TYPE::DOUBLE_64)
{
    char *ptr = reinterpret_cast<char*>(&data);
    for (uint i = 0; i < sizeof(data); i++)
        _data.push_back(ptr[i]);
}

Raw::Raw(bool data) :
    _type(DATA_TYPE::BOOL),
    _data{1, data}
{

}

Raw::Raw(QString data) :
    _type(DATA_TYPE::STRING),
    _data{data.toStdString().c_str(), static_cast<qsizetype>(data.toStdString().size())}
{

}

Raw::Raw(QByteArray data) :
    _type(DATA_TYPE::RAW),
    _data(data)
{

}

Raw::Raw(DATA_TYPE type) : _type(type)
{

}

Raw::Raw(DATA_TYPE type, const QByteArray &data) : _type(type), _data(data)
{

}

bool Raw::isZeroOnly() const
{
    if (!isValid())
        return false;

    for (auto el : _data)
    {
        if (el != 0)
            return false;
    }

    return true;
}

bool Raw::isValid() const
{
    if (type() == DATA_TYPE::BOOL || type() == DATA_TYPE::INT_8)
        return _data.size() == sizeof(int8_t);
    if (type() == DATA_TYPE::INT_16)
        return _data.size() == sizeof(int16_t);
    if (type() == DATA_TYPE::INT_32)
        return _data.size() == sizeof(int32_t);
    if (type() == DATA_TYPE::INT_64)
        return _data.size() == sizeof(int64_t);

    if (type() == DATA_TYPE::FLOAT_32)
        return _data.size() == sizeof(float);
    if (type() == DATA_TYPE::DOUBLE_64)
        return _data.size() == sizeof(double);

    if (type() == DATA_TYPE::NONE)
        return false;

    return true;
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

std::pair<QString, QString> Raw::strData() const
{
    std::pair<QString, QString> result;

    switch (_type)
    {
    case DATA_TYPE::INT_8:
        result = std::make_pair<QString, QString>(QString::number(*reinterpret_cast<const int8_t*>(_data.data())), "INT_8");
        break;
    case DATA_TYPE::INT_16:
        result = std::make_pair<QString, QString>(QString::number(*reinterpret_cast<const int16_t*>(_data.data())), "INT_16");
        break;
    case DATA_TYPE::INT_32:
        result = std::make_pair<QString, QString>(QString::number(*reinterpret_cast<const int32_t*>(_data.data())), "INT_32");
        break;
    case DATA_TYPE::INT_64:
        result = std::make_pair<QString, QString>(QString::number(*reinterpret_cast<const qint64*>(_data.data())), "INT_64");
        break;
    case DATA_TYPE::FLOAT_32:
        result = std::make_pair<QString, QString>(QString::number(*reinterpret_cast<const float*>(_data.data()), 'l', 4), "FLOAT_32");
        break;
    case DATA_TYPE::DOUBLE_64:
        result = std::make_pair<QString, QString>(QString::number(*reinterpret_cast<const double*>(_data.data()), 'l', 4), "DOUBLE_64");
        break;
    case DATA_TYPE::BOOL:
        result = std::make_pair<QString, QString>(*reinterpret_cast<const bool*>(_data.data()) ? "true" : "false", "BOOL");
        break;
    case DATA_TYPE::STRING:
        result = std::make_pair<QString, QString>(_data.data(), "STRING");
        break;
    case DATA_TYPE::RAW:
        result = std::make_pair<QString, QString>(_data.toHex(':'), "RAW");
        break;
    default:
        result = std::make_pair<QString, QString>("", "NONE");
        break;
    }

    return result;
}

std::pair<QString, QString> Raw::strData(const QByteArray &data, DATA_TYPE type)
{
    Raw raw(type, data);
    return raw.strData();
}

QByteArray Raw::strToByteArray(const QString &dataStr, DATA_TYPE type)
{
    QByteArray result;
    bool ok;

    if (type == DATA_TYPE::INT_8)
    {
        int8_t data = dataStr.toLongLong(&ok);
        if (!ok)
        {
            qDebug() << "Convert to INT_8 error";
            return {};
        }
        result.push_back(data);
    }
    else if (type == DATA_TYPE::INT_16)
    {
        int16_t data = dataStr.toLongLong(&ok);
        if (!ok)
        {
            qDebug() << "Convert to INT_16 error";
            return {};
        }
        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            data = qToLittleEndian(data);

        char *ptr = reinterpret_cast<char*>(&data);
        for (uint i = 0; i < sizeof(data); i++)
            result.push_back(ptr[i]);
    }
    else if (type == DATA_TYPE::INT_32)
    {
        int32_t data = dataStr.toLongLong(&ok);
        if (!ok)
        {
            qDebug() << "Convert to INT_32 error";
            return {};
        }
        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            data = qToLittleEndian(data);

        char *ptr = reinterpret_cast<char*>(&data);
        for (uint i = 0; i < sizeof(data); i++)
            result.push_back(ptr[i]);
    }
    else if (type == DATA_TYPE::INT_64)
    {
        qint64 data = dataStr.toLongLong(&ok);
        if (!ok)
        {
            qDebug() << "Convert to INT_64 error";
            return {};
        }
        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            data = qToLittleEndian(data);

        char *ptr = reinterpret_cast<char*>(&data);
        for (uint i = 0; i < sizeof(data); i++)
            result.push_back(ptr[i]);
    }
    else if (type == DATA_TYPE::FLOAT_32)
    {
        float data = dataStr.toFloat(&ok);
        if (!ok)
        {
            qDebug() << "Convert to FLOAT_32 error";
            return {};
        }
        char *ptr = reinterpret_cast<char*>(&data);
        for (uint i = 0; i < sizeof(data); i++)
            result.push_back(ptr[i]);
    }
    else if (type == DATA_TYPE::DOUBLE_64)
    {
        double data = dataStr.toDouble(&ok);
        if (!ok)
        {
            qDebug() << "Convert to DOUBLE_64 error";
            return {};
        }
        char *ptr = reinterpret_cast<char*>(&data);
        for (uint i = 0; i < sizeof(data); i++)
            result.push_back(ptr[i]);
    }
    else if (type == DATA_TYPE::BOOL)
    {
        if (dataStr.toUpper() == "TRUE")
            result.push_back(true);
        else if ((dataStr.toUpper() == "FALSE"))
            result.push_back(false);
        else
        {
            bool data = dataStr.toInt(&ok);
            if (!ok)
            {
                qDebug() << "Convert to BOOL error";
                return {};
            }
            result.push_back(data);
        }
    }
    else if (type == DATA_TYPE::RAW || type == DATA_TYPE::STRING)
        result.append(dataStr.toLocal8Bit());

    return result;
}

bool Raw::boolType() const
{
    return type() == DATA_TYPE::BOOL;
}

bool Raw::intType() const
{
    if (type() == DATA_TYPE::INT_8 ||
            type() == DATA_TYPE::INT_16 ||
            type() == DATA_TYPE::INT_32 ||
            type() == DATA_TYPE::INT_64)
        return true;

    return false;
}

bool Raw::realType() const
{
    if (type() == DATA_TYPE::FLOAT_32 || type() == DATA_TYPE::DOUBLE_64)
        return true;

    return false;
}

bool Raw::stringType() const
{
    return type() == DATA_TYPE::STRING;
}

bool Raw::rawType() const
{
    return type() == DATA_TYPE::RAW;
}

bool Raw::noneType() const
{
    return type() == DATA_TYPE::NONE;
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

template <typename C>
bool compare(const Raw &lhs, const Raw &rhs, C cmp = C{})
{
    if ((!lhs.noneType() || !rhs.noneType() ) &&
            (!lhs.isValid() || !rhs.isValid()))
        return false;

    if (lhs.type() == rhs.type())
    {
        if (lhs.type() == Raw::DATA_TYPE::BOOL)
        {
            bool lhsArg = lhs.data().at(0);
            bool rhsArg = rhs.data().at(0);
            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::INT_8)
        {
            int8_t lhsArg = lhs.data().at(0);
            int8_t rhsArg = rhs.data().at(0);
            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::INT_16)
        {
            int16_t lhsArg, rhsArg;

            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

            if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            {
                lhsArg = qToLittleEndian(lhsArg);
                rhsArg = qToLittleEndian(rhsArg);
            }

            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::INT_32)
        {
            int32_t lhsArg, rhsArg;

            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

            if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            {
                lhsArg = qToLittleEndian(lhsArg);
                rhsArg = qToLittleEndian(rhsArg);
            }

            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::INT_64)
        {
            int64_t lhsArg, rhsArg;

            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

            if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            {
                lhsArg = qToLittleEndian(lhsArg);
                rhsArg = qToLittleEndian(rhsArg);
            }

            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::FLOAT_32)
        {
            float lhsArg, rhsArg;
            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::DOUBLE_64)
        {
            double lhsArg, rhsArg;
            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::STRING || lhs.type() == Raw::DATA_TYPE::RAW || lhs.type() == Raw::DATA_TYPE::NONE)
        {
            return cmp(lhs.data(), rhs.data());
        }
    }
    else
    {
        if ((lhs.intType() || lhs.realType() || lhs.boolType()) && (rhs.intType() || rhs.realType() || rhs.boolType()))
        {
            bool ok1, ok2;
            double lhsArg = lhs.strData().first.toDouble(&ok1);
            double rhsArg = rhs.strData().first.toDouble(&ok2);

            if (!ok1 || !ok2)
                return false;

            return cmp(lhsArg, rhsArg);
        }
    }

    return false;
}

template <typename T>
Raw operation(const Raw &lhs, const Raw &rhs, T op = T{})
{
    if (!lhs.isValid() || !rhs.isValid())
        return {};

    if (lhs.type() == rhs.type())
    {
        if ((lhs.intType() || lhs.realType() || lhs.boolType()) && (rhs.intType() || rhs.realType() || rhs.boolType()))
        {
            bool ok1, ok2;
            double lhsArg = lhs.strData().first.toDouble(&ok1);
            double rhsArg = rhs.strData().first.toDouble(&ok2);

            if (!ok1 || !ok2)
                return {};

            double result = op(lhsArg, rhsArg);
            return result;
        }


//        if (lhs.type() == Raw::DATA_TYPE::BOOL)
//        {
//            bool lhsArg = lhs.data().at(0);
//            bool rhsArg = rhs.data().at(0);

//            return op(lhsArg, rhsArg);
//        }
//        else if (lhs.type() == Raw::DATA_TYPE::INT_8)
//        {
//            int8_t lhsArg = lhs.data().at(0);
//            int8_t rhsArg = rhs.data().at(0);
//            int8_t result = op(lhsArg, rhsArg);

//            return {result};
//        }
//        else if (lhs.type() == Raw::DATA_TYPE::INT_16)
//        {
//            int16_t lhsArg, rhsArg;

//            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
//            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

//            if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
//            {
//                lhsArg = qToLittleEndian(lhsArg);
//                rhsArg = qToLittleEndian(rhsArg);
//            }
//            int16_t result = op(lhsArg, rhsArg);

//            return {result};
//        }
//        else if (lhs.type() == Raw::DATA_TYPE::INT_32)
//        {
//            int32_t lhsArg, rhsArg;

//            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
//            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

//            if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
//            {
//                lhsArg = qToLittleEndian(lhsArg);
//                rhsArg = qToLittleEndian(rhsArg);
//            }
//            int32_t result = op(lhsArg, rhsArg);

//            return {result};
//        }
//        else if (lhs.type() == Raw::DATA_TYPE::INT_64)
//        {
//            int64_t lhsArg, rhsArg;

//            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
//            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

//            if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
//            {
//                lhsArg = qToLittleEndian(lhsArg);
//                rhsArg = qToLittleEndian(rhsArg);
//            }
//            int64_t result = op(lhsArg, rhsArg);

//            return {result};
//        }
//        else if (lhs.type() == Raw::DATA_TYPE::FLOAT_32)
//        {
//            float lhsArg, rhsArg;

//            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
//            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

//            float result = op(lhsArg, rhsArg);

//            return {result};
//        }
//        else if (lhs.type() == Raw::DATA_TYPE::DOUBLE_64)
//        {
//            double lhsArg, rhsArg;

//            std::memcpy(&lhsArg, lhs.data().data(), lhs.data().size());
//            std::memcpy(&rhsArg, rhs.data().data(), rhs.data().size());

//            double result = op(lhsArg, rhsArg);

//            return {result};
//        }
        else if (lhs.type() == Raw::DATA_TYPE::STRING || lhs.type() == Raw::DATA_TYPE::RAW || lhs.type() == Raw::DATA_TYPE::NONE)
        {
            if (typeid(op) == typeid(std::plus<>))
                return {lhs.type(), lhs.data() + rhs.data()};

            return {};
        }
    }
    else
    {
        if ((lhs.intType() || lhs.realType() || lhs.boolType()) && (rhs.intType() || rhs.realType() || rhs.boolType()))
        {
            bool ok1, ok2;
            double lhsArg = lhs.strData().first.toDouble(&ok1);
            double rhsArg = rhs.strData().first.toDouble(&ok2);

            if (!ok1 || !ok2)
                return {};

            double result = op(lhsArg, rhsArg);
            return result;
        }
    }

    return {};
}

bool operator==(const Raw &lhs, const Raw &rhs)
{
    return compare(lhs, rhs, std::equal_to<>{});
}

bool operator!=(const Raw &lhs, const Raw &rhs)
{
    return compare(lhs, rhs, std::not_equal_to<>{});
}

bool operator>(const Raw &lhs, const Raw &rhs)
{
    return compare(lhs, rhs, std::greater<>{});
}

bool operator<(const Raw &lhs, const Raw &rhs)
{
    return compare(lhs, rhs, std::less<>{});
}

bool operator>=(const Raw &lhs, const Raw &rhs)
{
    return compare(lhs, rhs, std::greater_equal<>{});
}

bool operator<=(const Raw &lhs, const Raw &rhs)
{
    return compare(lhs, rhs, std::less_equal<>{});
}

Raw operator+(const Raw &lhs, const Raw &rhs)
{
    return operation(lhs, rhs, std::plus<>{});
}

Raw operator-(const Raw &lhs, const Raw &rhs)
{
    return operation(lhs, rhs, std::minus<>{});
}

Raw operator*(const Raw &lhs, const Raw &rhs)
{
    return operation(lhs, rhs, std::multiplies<>{});
}

Raw operator/(const Raw &lhs, const Raw &rhs)
{
    if (rhs.isZeroOnly())
        return {};

    return operation(lhs, rhs, std::divides<>{});
}
