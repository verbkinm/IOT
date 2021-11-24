#ifndef IOTV_SH_CLASS_H
#define IOTV_SH_CLASS_H

#include <QByteArray>
#include "lib/Base_Host/base_host.h"

#define QUERY_WAY_BYTE 0x01
#define QUERY_READ_BYTE 0x02
#define QUERY_WRITE_BYTE 0x00

#define RESPONSE_WAY_BYTE 0x05
#define RESPONSE_READ_BYTE 0x06
#define RESPONSE_WRITE_BYTE 0x04

class IOTV_SH
{
public:
    enum class Response_Type
    {
        RESPONSE_WAY,
        RESPONSE_READ,
        RESPONSE_WRITE,
        ERROR
    };

    static void query_WAY(QByteArray &data);
    static qint64 query_READ(Base_Host &host, uint8_t channelNumber);
    static qint64 query_WRITE(Base_Host &host, uint8_t channelNumber, Raw::RAW &rawData);

    static void response_WAY(Base_Host &iotHost, const QByteArray &data);
    static void response_READ(Base_Host &iotHost, const QByteArray &data);
    static void response_WRITE(Base_Host &iotHost, const QByteArray &data);

    static Response_Type checkResponsetData(const QByteArray &data);

    static uint8_t channelNumber(char byte);

    static std::pair<bool, int> accumPacket(const QByteArray &data);
};

#endif // IOTV_SH_CLASS_H
