#pragma once

#include <cstdint>

#include <QByteArray>
#include <QString>
#include <QVariant>

#include "raw_global.h"

class RAW_EXPORT Raw
{
public:
    enum class DATA_TYPE : uint8_t
    {
        INTEGER_8 = 0x00,
        INTEGER_16 = 0x01,
        INTEGER_32 = 0x02,
        INTEGER_64 = 0x03,

        UNSIGNED_INTEGER_8 = 0x04,
        UNSIGNED_INTEGER_16 = 0x05,
        UNSIGNED_INTEGER_32 = 0x06,
        UNSIGNED_INTEGER_64 = 0x07,

        FLOAT_32 = 0x08,
        DOUBLE_32 = 0x09,
        DOUBLE_64 = 0x0A,

        BOOL_8 = 0x0B,
        STRING = 0x0C,

        RAW = 0x0D,
        NONE = 0x0E
    };

    Raw();
    Raw(DATA_TYPE type);
    Raw(DATA_TYPE type, const QByteArray &data);

    friend bool operator==(const Raw &lhs, const Raw &rhs);

    uint16_t size() const;

    void setType(DATA_TYPE newType);
    void setData(const QByteArray &newData);

    void push_back(uint8_t byte);
    void clear();

    DATA_TYPE type() const;
    const QByteArray &data() const;

    QString strData() const;


private:
    DATA_TYPE _type;
    QByteArray _data;
};
