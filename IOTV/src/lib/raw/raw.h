#pragma once

#include <cstdint>

#include <QByteArray>
#include <QtEndian>

#include <QDebug>

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

    Raw();

    Raw(int8_t data);
    Raw(int16_t data);
    Raw(int32_t data);
    Raw(int64_t data);

    Raw(float data);
    Raw(double data);

    Raw(bool data);

    Raw(QString data);
    Raw(QByteArray data);

    Raw(DATA_TYPE type);
    Raw(DATA_TYPE type, const QByteArray &data);

    bool isZeroOnly() const;
    inline bool boolType() const;
    inline bool intType() const;
    inline bool realType() const;
    inline bool stringType() const;
    inline bool rawType() const;
    inline bool noneType() const;

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

    uint16_t size() const;

    void setType(DATA_TYPE newType);
    void setData(const QByteArray &newData);

    void push_back(uint8_t byte);
    void clear();

    DATA_TYPE type() const;
    const QByteArray &data() const;

    std::pair<QString, QString> strData() const;

    static std::pair<QString, QString> strData(const QByteArray &data, DATA_TYPE type);
    static QByteArray strToByteArray(const QString &dataStr, DATA_TYPE type);

private:
    DATA_TYPE _type;
    QByteArray _data;
};
