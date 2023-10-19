#pragma once

#include <cstdint>
#include <unordered_map>

#include <QByteArray>
#include <QVariant>
#include <QtEndian>

#include <QDebug>

#include "ConfigTypes.h"

class Raw
{
public:
    enum class DATA_TYPE : uint8_t
    {
        INT_8 = 0,
        INT_16,
        INT_32,
        INT_64,

        FLOAT_32,
        DOUBLE_64, // на МК double 32-битный может быть

        BOOL,
        STRING,

        RAW,
        NONE
    };
    static const std::unordered_map <QString, Raw::DATA_TYPE> typeOnString;

    Raw();

    explicit Raw(int8_t data);
    explicit Raw(int16_t data);
    explicit Raw(int32_t data);
    explicit Raw(int64_t data);

    explicit Raw(float data);
    explicit Raw(double data);

    explicit Raw(bool data);

    explicit Raw(const QString &data);
    explicit Raw(const QByteArray &data);

    explicit Raw(DATA_TYPE type);
    explicit Raw(DATA_TYPE type, const QByteArray &data);
    explicit Raw(DATA_TYPE type, const QString &data);

    //! Массив данных полностью состоит их нулей
    bool isZeroOnly() const;

    //! Длина данных соответствует типу. String и Raw всегда истинны, None всегда ложь
    bool isValid() const;

    bool isBool() const;
    bool isInt() const;
    bool isReal() const;
    bool isString() const;
    bool isRawType() const;
    bool isNoneType() const;

    friend bool operator==(const Raw &lhs, const Raw &rhs);
    friend bool operator!=(const Raw &lhs, const Raw &rhs);
    friend bool operator>(const Raw &lhs, const Raw &rhs);
    friend bool operator<(const Raw &lhs, const Raw &rhs);
    friend bool operator>=(const Raw &lhs, const Raw &rhs);
    friend bool operator<=(const Raw &lhs, const Raw &rhs);

    friend Raw operator+(const Raw &lhs, const Raw &rhs);
    friend Raw operator-(const Raw &lhs, const Raw &rhs);
    friend Raw operator*(const Raw &lhs, const Raw &rhs);
    friend Raw operator/(const Raw &lhs, const Raw &rhs);

//    friend std::ostream &operator<<(std::ostream& os, const Raw &raw);

    uint16_t size() const;

    void setType(DATA_TYPE newType);
    void setData(const QByteArray &newData);
    void addData(const QByteArray &newData);

    void push_back(uint8_t byte);
    void clear();

    DATA_TYPE type() const;
    const QByteArray &data() const;

    std::pair<QString, QString> strData() const;

    static std::pair<QString, QString> strData(const QByteArray &data, DATA_TYPE type);
    static QByteArray strToByteArray(const QString &dataStr, DATA_TYPE type);
    static DATA_TYPE dataType(const QString &str);
    static QString strType(DATA_TYPE type);

private:
    DATA_TYPE _type;
    QByteArray _data;
};
