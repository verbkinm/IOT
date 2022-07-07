#pragma once

#include <QByteArray>
#include "base_host.h"

class IOTV_SH
{
public:
    enum class Response_Type : uint8_t
    {
        RESPONSE_WAY,
        RESPONSE_READ,
        RESPONSE_WRITE,
        RESPONSE_PONG,
        ERROR
    };

    enum : uint8_t
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

    static QByteArray query_WAY();
    static QByteArray query_READ(uint8_t channelNumber);
    static QByteArray query_WRITE(const Base_Host &host, uint8_t channelNumber, const Raw::RAW &rawData);
    static QByteArray query_PING();

    static void response_WAY(Base_Host &iotHost, const QByteArray &data);
    static void response_READ(Base_Host &iotHost, const QByteArray &data);
    static void response_WRITE(const Base_Host &iotHost, const QByteArray &data);
    static void response_PONG(QByteArray &data);

    static Response_Type checkResponsetData(const QByteArray &data);

    static uint8_t channelNumber(uint8_t byte);

    static std::pair<bool, uint32_t> accumPacket(const QByteArray &data);
};

