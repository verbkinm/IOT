#pragma once

#include <QByteArrayList>


#define MAX_LENGTH_DEVICE_NAME 31

class IOTV_SC
{
public:
    enum class ByteType : uint8_t
    {
        RESPONSE_DEVICE_LIST_BYTE = 0x05,
        //First byte
        DEVICE_BIT = 0x01,
        READ_WRITE_BIT = 0x02,
        QUERY_RESPONSE_BIT = 0x04,

        RESPONCE_READ = 0x06,
        RESPONCE_WRITE = QUERY_RESPONSE_BIT,

        //Second byte
        DEVICE_STATE_QUERY_BIT = 0x10,
        DEVICE_STATE_RESPONSE_BIT = 0x20
    };



    enum class Response_Type : uint8_t
    {
        RESPONSE_DEVICE_LIST,
        RESPONSE_STATE,
        RESPONSE_READ,
        RESPONSE_WRITE,
        RESPONSE_INCOMPLETE,
        RESPONSE_ERROR
    };

    // генерация данных для запросов
    static QByteArray query_Device_List();
    static QByteArray query_STATE(const QString &deviceName);
    static QByteArray query_READ(const QString &deviceName, uint8_t channelNumber);
    static QByteArray query_WRITE(const QString &deviceName, uint8_t channelNumber, const QByteArray &rawData);

    //
    static QByteArrayList response_Device_List(const QByteArray &data);
    static void serverResponse_STATE(Base_Host &host, const QByteArray &data);
    static void serverResponse_READ(Base_Host &host, const QByteArray &data);
    static void serverResponse_WRITE(Base_Host &host, const QByteArray &data);

    static void responceToClient_Device_One(Base_Host &host, QByteArray &data);
    static void responceToClient_State(const Base_Host &host, QByteArray &data);
    static void responceToClient_Read(const Base_Host &host, QByteArray &data);
    static void responceToClient_Write(QByteArray &data);


    inline static uint8_t channelNumber(char byte);
    static bool responseName(const QByteArray &data, QString &returnName);
    static bool queryName(const QByteArray &data, QString &returnName);

    static std::pair<bool, int> accumResponcePacket(const QByteArray &data);
};
