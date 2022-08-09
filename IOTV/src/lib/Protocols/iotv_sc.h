#pragma once

#include <QByteArrayList>

#define RESPONSE_DEVICE_LIST_BYTE 0x05

//First byte
#define DEVICE_BIT 0x01
#define READ_WRITE_BIT 0x02
#define QUERY_RESPONSE_BIT 0x04

#define RESPONCE_READ 0x06
#define RESPONCE_WRITE 0x04

//Second byte
#define DEVICE_STATE_QUERY_BIT 0x10
#define DEVICE_STATE_RESPONSE_BIT 0x20

#define MAX_LENGTH_DEVICE_NAME 31

class IOTV_SC
{
public:
    static QByteArray query_Device_List();
    static QByteArray query_STATE(const QString &deviceName);
    static QByteArray query_READ(const QString &deviceName, uint8_t channelNumber);
    static QByteArray query_WRITE(const QString &deviceName, uint8_t channelNumber, const QByteArray &rawData);

    static QByteArrayList response_Device_List(const QByteArray &data);
    static void serverResponse_STATE(Base_Host &host, const QByteArray &data);
    static void serverResponse_READ(Base_Host &host, const QByteArray &data);
    static void serverResponse_WRITE(Base_Host &host, const QByteArray &data);

    static void responceToClient_Device_One(Base_Host &host, QByteArray &data);
    static void responceToClient_State(const Base_Host &host, QByteArray &data);
    static void responceToClient_Read(const Base_Host &host, QByteArray &data);
    static void responceToClient_Write(QByteArray &data);

    enum class Query_Type
    {
        QUERY_DEVICE_LIST,
        QUERY_STATE,
        QUERY_READ,
        QUERY_WRITE,
        QUERY_ERROR
    };
    enum class Response_Type
    {
        RESPONSE_DEVICE_LIST,
        RESPONSE_STATE,
        RESPONSE_READ,
        RESPONSE_WRITE,
        RESPONSE_ERROR
    };

    static Query_Type checkQueryData(const QByteArray &data);
    static Response_Type checkResponsetData(const QByteArray &data);

    inline static uint8_t channelNumber(char byte);
    static bool responseName(const QByteArray &data, QString &returnName);
    static bool queryName(const QByteArray &data, QString &returnName);

    static std::pair<bool, int> accumResponcePacket(const QByteArray &data);
    static std::pair<bool, int> accumQueryPacket(const QByteArray &data);
};
