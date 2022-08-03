#pragma once

#include <vector>

#include "raw_global.h"

class RAW_EXPORT Raw
{
public:
    enum class DATA_TYPE : char
    {
        INTEGER_8,
        INTEGER_16,
        INTEGER_32,
        INTEGER_64,

        UNSIGNED_INTEGER_8,
        UNSIGNED_INTEGER_16,
        UNSIGNED_INTEGER_32,
        UNSIGNED_INTEGER_64,

        FLOAT_32,
        DOUBLE_32,
        DOUBLE_64,

        BOOL_8,
        CHAR_PTR,

        RAW
    };

    Raw();

    static DATA_TYPE toDataType(char type);
    static char toUInt8(DATA_TYPE dataType);

    friend bool operator==(const Raw &lhs, const Raw &rhs);

    unsigned short size() const;

    void setType(DATA_TYPE newType);
    void setData(std::vector<char> &newData);
    void push_back(char byte);

    DATA_TYPE type() const;
    const std::vector<char> &data() const;

private:
    DATA_TYPE _type;
    std::vector<char> _data;
};
