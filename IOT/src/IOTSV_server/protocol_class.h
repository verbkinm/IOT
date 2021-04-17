#ifndef PROTOCOL_CLASS_H
#define PROTOCOL_CLASS_H

#include "IOT_Host/IOT_Host.h"

#define QUERY_WAY 0b00001001
#define QUERY_READ 0b00001010
#define QUERY_WRITE 0b00001000

class Protocol_Class
{

private:
    enum First_Byte
    {
        WAY = 0x1,
        READ_WRITE = 0x2, //0 - write, 1 - read
        RESPONSE = 0x4,
        QUERY = 0x8
    };

public:
    enum class Response_Type
    {
        RESPONSE_WAY,
        RESPONSE_READ,
        RESPONSE_WRITE,
        ERROR
    };

    Q_DECLARE_FLAGS(First_Byte_Flags, First_Byte)

    static void query_WAY(QByteArray &data);
    static void query_READ(QByteArray &data, uint8_t channelNumber);
    static void query_WRITE(QByteArray &data, uint8_t channelNumber, Raw::RAW writeData);

    static void response_WAY(IOT_Host* iotHost, const QByteArray &data);
    static void response_READ(IOT_Host* iotHost, const QByteArray &data);
    static void response_WRITE(IOT_Host* iotHost, const QByteArray &data);

    static Response_Type checkResponsetData(const QByteArray &data);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Protocol_Class::First_Byte_Flags)

#endif // PROTOCOL_CLASS_H
