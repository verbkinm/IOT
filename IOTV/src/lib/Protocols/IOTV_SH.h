#pragma once

#include <QByteArray>
#include "raw.h"

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

    struct RESPONSE_PKG
    {
        Response_Type type = Response_Type::ERROR;
    };

    struct RESPONSE_WAY : RESPONSE_PKG
    {
        RESPONSE_WAY()
        {
            type = Response_Type::RESPONSE_WAY;
        }

        uint8_t id;
        std::string description;
        std::vector<Raw::DATA_TYPE> readChannel;
        std::vector<Raw::DATA_TYPE> writeChannel;
    };

    struct RESPONSE_WRITE : RESPONSE_PKG
    {
        RESPONSE_WRITE()
        {
            type = Response_Type::RESPONSE_WRITE;
        }

        uint8_t chanelNumber;
    };

    struct RESPONSE_READ : RESPONSE_WRITE
    {
        RESPONSE_READ()
        {
            type = Response_Type::RESPONSE_READ;
        }

        QByteArray data;
    };

    struct RESPONSE_PONG : RESPONSE_PKG
    {
        RESPONSE_PONG()
        {
            type = Response_Type::RESPONSE_PONG;
        }

        bool state;
    };

    static QByteArray query_WAY();
    static QByteArray query_READ(uint8_t channelNumber);
    static QByteArray query_WRITE(uint8_t channelNumber, const Raw &rawData);
    static QByteArray query_PING();

    static uint8_t channelNumber(uint8_t byte);

    static RESPONSE_PKG accumPacket(QByteArray &data);

    static QByteArray vecUInt8ToQByteArray(const std::vector<uint8_t> &vec);
    static std::vector<uint8_t> QByteArrayToVecUInt8(const QByteArray &arr);

private:
    static RESPONSE_PKG createResponse_WAY_PKG(QByteArray &data);
    static RESPONSE_PKG createResponse_READ_PKG(QByteArray &data);
    static RESPONSE_PKG createResponse_WRITE_PKG(QByteArray &data);
    static RESPONSE_PKG createResponse_PONG_PKG(QByteArray &data);
};

