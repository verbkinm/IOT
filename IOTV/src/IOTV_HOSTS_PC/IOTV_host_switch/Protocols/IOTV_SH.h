#pragma once

#include <stdint.h>
#include <cstring>

class IOT_Server;

class Protocol_class
{
public:
    //Принята BigEndian последовательность в коде
    enum DATA_TYPE
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

    enum
    {
        QUERY_WAY_BYTE = 0x01,
        QUERY_READ_BYTE = 0x02,
        QUERY_WRITE_BYTE = 0x00,
        QUERY_PING_BYTE = 0x08,

        RESPONSE_WAY_BYTE = 0x05,
        RESPONSE_READ_BYTE = 0x06,
        RESPONSE_WRITE_BYTE = 0x04,
        RESPONSE_PONG_BYTE = 0x0C
    };

    static uint16_t response_WAY(const IOT_Server &iotHost, char *outData);
    static uint16_t response_READ(const IOT_Server &iotHost, const char *inData, const char* ptrInData, char *outData);
    static uint16_t response_WRITE(IOT_Server &iotHost, const char *inData, const char *ptrInData, char *outData);
    static uint16_t response_Pong(char *outData);
};
