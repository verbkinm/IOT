#pragma once

#include <vector>
#include <cstdint>

#include "raw_global.h"

class RAW_EXPORT Raw
{
public:
    enum class DATA_TYPE : uint8_t
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
        STRING,

        RAW,
        NONE
    };

    Raw();
    Raw(DATA_TYPE type);
    Raw(DATA_TYPE type, const std::vector<uint8_t> &data);

    friend bool operator==(const Raw &lhs, const Raw &rhs);

    uint16_t size() const;

    void setType(DATA_TYPE newType);
    void setData(const std::vector<uint8_t> &newData);

    void push_back(uint8_t byte);
    void clear();

    DATA_TYPE type() const;
    const std::vector<uint8_t> &data() const;

private:
    DATA_TYPE _type;
    std::vector<uint8_t> _data;
};
