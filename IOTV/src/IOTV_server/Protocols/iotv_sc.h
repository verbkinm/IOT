#ifndef IOTV_SC_H
#define IOTV_SC_H

#include "IOT_Server/iot_server.h"

//First byte
#define DEVICE_BIT 0x01
#define READ_WRITE_BIT 0x02
#define QUERY_RESPONSE_BIT 0x04
//Second byte
#define DEVICE_STATE_QUERY_BIT 0x10

class IOTV_SC
{
public:
    static void query_Device_List(QByteArray &data);
    static bool query_Device_State(QByteArray &data, const QString &deviceName);
    static bool query_READ(QByteArray &data, const QString &deviceName, uint8_t channelNumber);
    static bool query_WRITE(QByteArray &data, const QString &deviceName, uint8_t channelNumber, Raw::RAW writeData);

    static void response_Device_List(/*IOT_Server* iotServer,*/ const QByteArray &data);
    static void response_Device_State(/*IOT_Server* iotServer, */const QByteArray &data);
    static void response_READ(/*IOT_Server* iotServer, */const QByteArray &data);
    static void response_WRITE(/*IOT_Server* iotServer, */const QByteArray &data);

    static QByteArrayList splitQueryData(QByteArray &data);
    static QByteArrayList splitResponseData(QByteArray &data);

    enum class Query_Type
    {
        QUERY_DEVICE_LIST,
        QUERY_STATE,
        QUERY_READ,
        QUERY_WRITE,
        QUERY_ERROR
    };
    static Query_Type checkQueryData(const QByteArray &data);

    enum class Response_Type
    {
        RESPONSE_DEVICE_LIST,
        RESPONSE_STATE,
        RESPONSE_READ,
        RESPONSE_WRITE,
        RESPONSE_ERROR
    };
    static Response_Type checkResponsetData(const QByteArray &data);

    static uint8_t channelNumber(char byte);
};

#endif // IOTV_SC_H
