#pragma once

#include <stdint.h>
#include <vector>

#include "iot_server.h"

class Protocol_class
{
public:
    //Принята BigEndian последовательность в коде
    enum DATA_TYPE : uint8_t
    {
        INT_8,
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

    enum class query_type
    {
        QUERY_WAY,
        QUERY_READ,
        QUERY_WRITE,
        QUERY_PING,
        ERROR
    };

    static query_type checkQueryData(const std::vector<uint8_t> &data);

    static void response_WAY(const IOT_Server &iotHost, std::vector<uint8_t> &data);
    static void response_READ(const IOT_Server &iotHost, std::vector<uint8_t> &data);
    static void response_WRITE(IOT_Server &iotHost, std::vector<uint8_t> &data);
    static void response_Pong(std::vector<uint8_t> &data);
};
