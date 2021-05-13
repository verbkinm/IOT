#ifndef IOTV_SH_CLASS_H
#define IOTV_SH_CLASS_H

#include <QByteArray>
#include "base_host.h"
//#include "IOT_Host/IOT_Host.h"

#define QUERY_WAY 0x01
#define QUERY_READ 0x02
#define QUERY_WRITE 0x00

class IOTV_SH
{

//private:
//    enum First_Byte
//    {
//        WAY = 0x1,
//        READ_WRITE = 0x2, //0 - write, 1 - read
//        QUERY_RESPONSE = 0x4, //0 - query, 1 - response
//    };

public:
    enum class Response_Type
    {
        RESPONSE_WAY,
        RESPONSE_READ,
        RESPONSE_WRITE,
        ERROR
    };

//    Q_DECLARE_FLAGS(First_Byte_Flags, First_Byte)

    static void query_WAY(QByteArray &data);
    static void query_READ(QByteArray &data, uint8_t channelNumber);
    static void query_WRITE(QByteArray &data, uint8_t channelNumber, Raw::RAW writeData);

    static void response_WAY(Base_Host &iotHost, const QByteArray &data);
    static void response_READ(Base_Host &iotHost, const QByteArray &data);
    static void response_WRITE(Base_Host &iotHost, const QByteArray &data);

    static QByteArrayList splitResponseData(QByteArray &data);

    static Response_Type checkResponsetData(const QByteArray &data);

    static uint8_t channelNumber(char byte);
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(Protocol_Class::First_Byte_Flags)

#endif // IOTV_SH_CLASS_H
