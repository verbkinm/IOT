#include "raw.h"

const std::unordered_map <QString, Raw::DATA_TYPE> Raw::typeOnString = {{Json_Event_Action::DATA_TYPE_INT_8, Raw::DATA_TYPE::INT_8},
                                                                        {Json_Event_Action::DATA_TYPE_INT_16, Raw::DATA_TYPE::INT_16},
                                                                        {Json_Event_Action::DATA_TYPE_INT_32, Raw::DATA_TYPE::INT_32},
                                                                        {Json_Event_Action::DATA_TYPE_INT_64, Raw::DATA_TYPE::INT_64},
                                                                        {Json_Event_Action::DATA_TYPE_FLOAT_32, Raw::DATA_TYPE::FLOAT_32},
                                                                        {Json_Event_Action::DATA_TYPE_DOUBLE_64, Raw::DATA_TYPE::DOUBLE_64},
                                                                        {Json_Event_Action::DATA_TYPE_BOOL, Raw::DATA_TYPE::BOOL},
                                                                        {Json_Event_Action::DATA_TYPE_STRING, Raw::DATA_TYPE::STRING},
                                                                        {Json_Event_Action::DATA_TYPE_RAW, Raw::DATA_TYPE::RAW},
                                                                        {Json_Event_Action::DATA_TYPE_NONE, Raw::DATA_TYPE::NONE}};

Raw::Raw() : _type(DATA_TYPE::NONE)
{

}

Raw::Raw(int8_t data) :
    _type(DATA_TYPE::INT_8),
    _data{1, static_cast<char>(data)}
{

}

Raw::Raw(int16_t data) :
    _type(DATA_TYPE::INT_16)
{
    if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
        data = qToLittleEndian(data);

    _data.resize(sizeof(data));
    memcpy(_data.data(), &data, sizeof(data));
}

Raw::Raw(int32_t data) :
    _type(DATA_TYPE::INT_32)
{
    if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
        data = qToLittleEndian(data);

    _data.resize(sizeof(data));
    memcpy(_data.data(), &data, sizeof(data));
}

Raw::Raw(int64_t data) :
    _type(DATA_TYPE::INT_64)
{
    if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
        data = qToLittleEndian(data);

    _data.resize(sizeof(data));
    memcpy(_data.data(), &data, sizeof(data));
}
Raw::Raw(float data) :
    _type(DATA_TYPE::FLOAT_32)
{
    _data.resize(sizeof(data));
    memcpy(_data.data(), &data, sizeof(data));
}

Raw::Raw(double data) :
    _type(DATA_TYPE::DOUBLE_64)
{
    _data.resize(sizeof(data));
    memcpy(_data.data(), &data, sizeof(data));
}

Raw::Raw(bool data) :
    _type(DATA_TYPE::BOOL),
    _data{1, data}
{

}

Raw::Raw(const QString &data) :
    _type(DATA_TYPE::STRING),
    _data{data.toStdString().c_str(), static_cast<int>(data.toStdString().size())}
{

}

Raw::Raw(const QByteArray &data) :
    _type(DATA_TYPE::RAW),
    _data(data)
{

}

Raw::Raw(DATA_TYPE type) : _type(type)
{

}

Raw::Raw(DATA_TYPE type, const QByteArray &data)
{
    if (type == Raw::DATA_TYPE::BOOL)
    {
        if (data.size() > 0 && (data.at(0) == 0 || data.at(0) == '0' || data == "false"))
            *this = Raw(false);
        else
            *this = Raw(true);
    }
    else if (type == Raw::DATA_TYPE::INT_8)
    {
        if (data.size() != sizeof(int8_t))
        {
            *this = Raw{};
            return;
        }

        int8_t val = data.at(0);
        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::INT_16)
    {
        int16_t val;

        if (data.size() != sizeof(val))
            *this = Raw{};

        memcpy(&val, data.data(), sizeof(val));

        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            val = qToLittleEndian(val);

        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::INT_32)
    {
        int32_t val;

        if (data.size() != sizeof(val))
            *this = Raw{};
        memcpy(&val, data.data(), sizeof(val));

        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            val = qToLittleEndian(val);

        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::INT_64)
    {
        int64_t val;

        if (data.size() != sizeof(val))
            *this = Raw{};
        memcpy(&val, data.data(), sizeof(val));

        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            val = qToLittleEndian(val);

        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::FLOAT_32)
    {
        float val;

        if (data.size() != sizeof(val))
            *this = Raw{};
        memcpy(&val, data.data(), sizeof(val));

        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            val = qToLittleEndian(val);

        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::DOUBLE_64)
    {
        double val;

        if (data.size() != sizeof(val))
            *this = Raw{};
        memcpy(&val, data.data(), sizeof(val));

        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            val = qToLittleEndian(val);

        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::STRING)
    {
        QString str = data;
        *this = Raw(str);
    }
    else if (type == Raw::DATA_TYPE::RAW || type == Raw::DATA_TYPE::NONE)
    {
        this->setType(type);
        this->setData(data);
    }
}

Raw::Raw(DATA_TYPE type, const QString &data)
{
    if (type == Raw::DATA_TYPE::BOOL)
    {
        if (data.size() > 0 && (data == "0" || data == "false"))
            *this = Raw(false);
        else
            *this = Raw(true);
    }
    else if (type == Raw::DATA_TYPE::INT_8)
    {
        int8_t val = data.toInt();
        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::INT_16)
    {
        int16_t val = data.toInt();
        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::INT_32)
    {
        int32_t val = data.toInt();
        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::INT_64)
    {
        int64_t val = data.toLong();
        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::FLOAT_32)
    {
        float val = data.toFloat();
        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::DOUBLE_64)
    {
        double val = data.toDouble();
        *this = Raw(val);
    }
    else if (type == Raw::DATA_TYPE::STRING)
    {
        *this = Raw(data);
    }
    else if (type == Raw::DATA_TYPE::RAW || type == Raw::DATA_TYPE::NONE)
    {
        this->setType(type);
        this->setData(QByteArray(data.toStdString().c_str(), data.toStdString().size()));
    }
}

template <typename T>
void createOnVariant(T val, Raw &raw, bool ok)
{
    if (!ok)
        raw = Raw();
    else
        raw = Raw(val);
}

//Raw::Raw(DATA_TYPE type, const QVariant &variant)
//{
//    bool ok;
//    Raw result;

//    if (type == Raw::DATA_TYPE::BOOL)
//    {
//        QByteArray data = variant.toByteArray();
//        if (data.size() > 0 && (data.at(0) == 0 || data.at(0) == '0' || data == "false"))
//            result = Raw(false);
//        else
//            result = Raw(true);
//    }
//    else if (type == Raw::DATA_TYPE::INT_8)
//    {
//        int8_t val = variant.toInt(&ok);
//        createOnVariant(val, result, ok);
//    }
//    else if (type == Raw::DATA_TYPE::INT_16)
//    {
//        int16_t val = variant.toInt(&ok);
//        createOnVariant(val, result, ok);
//    }
//    else if (type == Raw::DATA_TYPE::INT_32)
//    {
//        int32_t val = variant.toInt(&ok);
//        createOnVariant(val, result, ok);
//    }
//    else if (type == Raw::DATA_TYPE::INT_64)
//    {
//        int64_t val = variant.toLongLong(&ok);
//        createOnVariant(val, result, ok);
//    }
//    else if (type == Raw::DATA_TYPE::FLOAT_32)
//    {
//        float val = variant.toFloat(&ok);
//        createOnVariant(val, result, ok);
//    }
//    else if (type == Raw::DATA_TYPE::DOUBLE_64)
//    {
//        double val = variant.toDouble(&ok);
//        createOnVariant(val, result, ok);
//    }
//    else if (type == Raw::DATA_TYPE::STRING)
//    {
//        result = Raw(variant.toString());
//    }
//    else if (type == Raw::DATA_TYPE::RAW || type == Raw::DATA_TYPE::NONE)
//    {
//        result = Raw(variant.toByteArray());
//    }

//    *this = result;
//}

bool Raw::isZeroOnly() const
{
    if (!isValid())
        return false;

    return std::all_of(_data.begin(), _data.end(), [](auto el){return el == 0;});
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

void Raw::addData(const QByteArray &newData)
{
    if (_type != DATA_TYPE::STRING && _type != DATA_TYPE::RAW)
        return;

    _data.append(newData.data(), newData.size());
}

std::pair<QString, QString> Raw::strData() const
{
    //!!! Если _data по длине не равняется типу данных, что произойдёт?
    switch (_type)
    {
    case DATA_TYPE::INT_8:
        return {QString::number(*reinterpret_cast<const int8_t*>(_data.data())), Json_Event_Action::DATA_TYPE_INT_8};
        break;
    case DATA_TYPE::INT_16:
        return {QString::number(*reinterpret_cast<const int16_t*>(_data.data())), Json_Event_Action::DATA_TYPE_INT_16};
        break;
    case DATA_TYPE::INT_32:
        return {QString::number(*reinterpret_cast<const int32_t*>(_data.data())), Json_Event_Action::DATA_TYPE_INT_32};
        break;
    case DATA_TYPE::INT_64:
        return {QString::number(*reinterpret_cast<const qint64*>(_data.data())), Json_Event_Action::DATA_TYPE_INT_64};
        break;
    case DATA_TYPE::FLOAT_32:
        return {QString::number(*reinterpret_cast<const float*>(_data.data()), 'l', 4), Json_Event_Action::DATA_TYPE_FLOAT_32};
        break;
    case DATA_TYPE::DOUBLE_64:
        return {QString::number(*reinterpret_cast<const double*>(_data.data()), 'l', 4), Json_Event_Action::DATA_TYPE_DOUBLE_64};
        break;
    case DATA_TYPE::BOOL:
        return {*reinterpret_cast<const bool*>(_data.data()) ? "true" : "false", Json_Event_Action::DATA_TYPE_BOOL};
        break;
    case DATA_TYPE::STRING:
        return {_data.data(), Json_Event_Action::DATA_TYPE_STRING};
        break;
    case DATA_TYPE::RAW:
        return {_data.toHex(':'), Json_Event_Action::DATA_TYPE_RAW};
        break;
    default:
        return {"", Json_Event_Action::DATA_TYPE_NONE};
        break;
    }

    return {};
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
        int8_t value = dataStr.toLongLong(&ok);
        if (!ok)
        {
            qDebug() << "Convert to INT_8 error";
            return {};
        }
        result.push_back(value);
    }
    else if (type == DATA_TYPE::INT_16)
    {
        int16_t value = dataStr.toLongLong(&ok);
        if (!ok)
        {
            qDebug() << "Convert to INT_16 error";
            return {};
        }
        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            value = qToLittleEndian(value);
        result.resize(sizeof(value));
        memcpy(result.data(), &value, sizeof(value));
//        char *ptr = reinterpret_cast<char*>(&value);
//        for (uint i = 0; i < sizeof(value); i++)
//            result.push_back(ptr[i]);
    }
    else if (type == DATA_TYPE::INT_32)
    {
        int32_t value = dataStr.toLongLong(&ok);
        if (!ok)
        {
            qDebug() << "Convert to INT_32 error";
            return {};
        }
        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            value = qToLittleEndian(value);
        result.resize(sizeof(value));
        memcpy(result.data(), &value, sizeof(value));
    }
    else if (type == DATA_TYPE::INT_64)
    {
        qint64 value = dataStr.toLongLong(&ok);
        if (!ok)
        {
            qDebug() << "Convert to INT_64 error";
            return {};
        }
        if (Q_BYTE_ORDER != Q_LITTLE_ENDIAN)
            value = qToLittleEndian(value);
        result.resize(sizeof(value));
        memcpy(result.data(), &value, sizeof(value));
    }
    else if (type == DATA_TYPE::FLOAT_32)
    {
        float value = dataStr.toFloat(&ok);
        if (!ok)
        {
            qDebug() << "Convert to FLOAT_32 error";
            return {};
        }
        result.resize(sizeof(value));
        memcpy(result.data(), &value, sizeof(value));
    }
    else if (type == DATA_TYPE::DOUBLE_64)
    {
        double value = dataStr.toDouble(&ok);
        if (!ok)
        {
            qDebug() << "Convert to DOUBLE_64 error";
            return {};
        }
        result.resize(sizeof(value));
        memcpy(result.data(), &value, sizeof(value));
    }
    else if (type == DATA_TYPE::BOOL)
    {
        if (dataStr.toUpper() == "TRUE")
            result.push_back(true);
        else if ((dataStr.toUpper() == "FALSE"))
            result.push_back(false);
        else
        {
            bool value = dataStr.toInt(&ok);
            if (!ok)
            {
                qDebug() << "Convert to BOOL error";
                return {};
            }
            result.push_back(value);
        }
    }
    else if (type == DATA_TYPE::RAW || type == DATA_TYPE::STRING)
        result.append(dataStr.toStdString().c_str());

    return result;
}

Raw::DATA_TYPE Raw::dataType(const QString &str)
{
    auto it = typeOnString.find(str);
    if (it != typeOnString.end())
        return it->second;

    return DATA_TYPE::NONE;
}

QString Raw::strType(DATA_TYPE type)
{
    switch (type)
    {
    case DATA_TYPE::INT_8:
        return Json_Event_Action::DATA_TYPE_INT_8;
    case DATA_TYPE::INT_16:
        return Json_Event_Action::DATA_TYPE_INT_16;
    case DATA_TYPE::INT_32:
        return Json_Event_Action::DATA_TYPE_INT_32;
    case DATA_TYPE::INT_64:
        return Json_Event_Action::DATA_TYPE_INT_64;
    case DATA_TYPE::FLOAT_32:
        return Json_Event_Action::DATA_TYPE_FLOAT_32;
    case DATA_TYPE::DOUBLE_64:
        return Json_Event_Action::DATA_TYPE_DOUBLE_64;
    case DATA_TYPE::BOOL:
        return Json_Event_Action::DATA_TYPE_BOOL;
    case DATA_TYPE::STRING:
        return Json_Event_Action::DATA_TYPE_STRING;
    case DATA_TYPE::RAW:
        return Json_Event_Action::DATA_TYPE_RAW;
    default:
        return Json_Event_Action::DATA_TYPE_NONE;
    }

    return {};
}

bool Raw::isBool() const
{
    return type() == DATA_TYPE::BOOL;
}

bool Raw::isInt() const
{
    if (type() == DATA_TYPE::INT_8 ||
            type() == DATA_TYPE::INT_16 ||
            type() == DATA_TYPE::INT_32 ||
            type() == DATA_TYPE::INT_64)
        return true;

    return false;
}

bool Raw::isReal() const
{
    if (type() == DATA_TYPE::FLOAT_32 || type() == DATA_TYPE::DOUBLE_64)
        return true;

    return false;
}

bool Raw::isString() const
{
    return type() == DATA_TYPE::STRING;
}

bool Raw::isRawType() const
{
    return type() == DATA_TYPE::RAW;
}

bool Raw::isNoneType() const
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
    if ((!lhs.isNoneType() || !rhs.isNoneType() ) &&
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

            memcpy(&lhsArg, lhs.data().data(), sizeof(lhsArg));
            memcpy(&rhsArg, rhs.data().data(), sizeof(rhsArg));

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
            memcpy(&lhsArg, lhs.data().data(), sizeof(lhsArg));
            memcpy(&rhsArg, rhs.data().data(), sizeof(rhsArg));

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
            memcpy(&lhsArg, lhs.data().data(), sizeof(lhsArg));
            memcpy(&rhsArg, rhs.data().data(), sizeof(rhsArg));

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
            memcpy(&lhsArg, lhs.data().data(), sizeof(lhsArg));
            memcpy(&rhsArg, rhs.data().data(), sizeof(rhsArg));

            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::DOUBLE_64)
        {
            double lhsArg, rhsArg;
            memcpy(&lhsArg, lhs.data().data(), sizeof(lhsArg));
            memcpy(&rhsArg, rhs.data().data(), sizeof(rhsArg));

            return cmp(lhsArg, rhsArg);
        }
        else if (lhs.type() == Raw::DATA_TYPE::STRING || lhs.type() == Raw::DATA_TYPE::RAW || lhs.type() == Raw::DATA_TYPE::NONE)
        {
            return cmp(lhs.data(), rhs.data());
        }
    }
    else
    {
        if ((lhs.isInt() || lhs.isReal() || lhs.isBool()) && (rhs.isInt() || rhs.isReal() || rhs.isBool()))
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
        if ((lhs.isInt() || lhs.isReal() || lhs.isBool()) && (rhs.isInt() || rhs.isReal() || rhs.isBool()))
        {
            bool ok1, ok2;
            double lhsArg = lhs.strData().first.toDouble(&ok1);
            double rhsArg = rhs.strData().first.toDouble(&ok2);

            if (!ok1 || !ok2)
                return {};

            double result = op(lhsArg, rhsArg);
            return Raw(result);
        }
        else if (lhs.type() == Raw::DATA_TYPE::STRING || lhs.type() == Raw::DATA_TYPE::RAW || lhs.type() == Raw::DATA_TYPE::NONE)
        {
            if (typeid(op) == typeid(std::plus<>))
                return Raw{lhs.type(), lhs.data() + rhs.data()};

            return {};
        }
    }
    else
    {
        if ((lhs.isInt() || lhs.isReal() || lhs.isBool()) && (rhs.isInt() || rhs.isReal() || rhs.isBool()))
        {
            bool ok1, ok2;
            double lhsArg = lhs.strData().first.toDouble(&ok1);
            double rhsArg = rhs.strData().first.toDouble(&ok2);

            if (!ok1 || !ok2)
                return {};

            double result = op(lhsArg, rhsArg);
            return Raw{result};
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

//std::ostream &operator<<(std::ostream& os, const Raw &raw)
//{
//    auto pair = raw.strData();
//    os << pair.first.toStdString() << ' ' << pair.second.toStdString();
//    return os;
//}
