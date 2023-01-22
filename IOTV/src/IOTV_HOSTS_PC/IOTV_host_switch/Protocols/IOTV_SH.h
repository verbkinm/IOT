#pragma once

#include <stdint.h>
#include <cstring>

class IOTV_Server;

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

    static uint16_t response_WAY(const IOTV_Server &iotHost, char *outData);
    static uint16_t response_READ(const IOTV_Server &iotHost, const char *inData, const char* ptrInData, char *outData);
    static int response_WRITE(IOTV_Server &iotHost, const char *inData, const char *ptrInData, char *outData);
    static uint16_t response_Pong(char *outData);
};

const static uint64_t HEADER_SIZE = 20;         // Заголовок протокола (20 байт + N байт, N максимум 248) (документация)
const static uint64_t IDENTIFICATION_SIZE = 16; // Идентификация устройства (16 + N байт, N максимум 2^40) (документация)
const static uint64_t READWRITE_SIZE = 15;      // Чтение / Запись данных (15 + N байт, N максимум 2^40) (документация)




