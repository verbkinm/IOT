#pragma once

#include <QByteArray>
#include <QString>

#include "raw.h"

/*
 * Обработка сырах данных, полученных клиентом от сервера
 */
enum : uint8_t
{
    QUERY_DEV_LIST_BYTE = 0x01,
    QUERY_STATE_FIRST_BYTE = 0x00,
    QUERY_STATE_BYTE = 0x10,
    QUERY_READ_BYTE = 0x02,
    QUERY_WRITE_BYTE = 0x00,

    //!!!
    RESPONSE_DEV_LIST_BYTE = 0x05,
    RESPONSE_STATE_BYTE = 0x10,
    RESPONSE_READ_BYTE = 0x02,
    RESPONSE_WRITE_BYTE = 0x00,
};

class Client_RX
{
public:
    enum class Response_Type : uint8_t
    {
        RESPONSE_DEVICE_LIST,
        RESPONSE_STATE,
        RESPONSE_READ,
        RESPONSE_WRITE,
        RESPONSE_INCOMPLETE,
        RESPONSE_ERROR
    };

    struct RESPONSE_PKG
    {
        RESPONSE_PKG(Response_Type qType = Response_Type::RESPONSE_ERROR) : type(qType)
        {
        }
        Response_Type type;
    };

    struct DEV_PKG
    {
        DEV_PKG() : id(0), error(true)
        {
        }

        uint8_t id;
        bool error;
        QString name;
        QString description;
        std::vector<Raw::DATA_TYPE> readChannel;
        std::vector<Raw::DATA_TYPE> writeChannel;

    };

    struct RESPONSE_DEV_LIST_PKG : RESPONSE_PKG
    {
        RESPONSE_DEV_LIST_PKG() : RESPONSE_PKG(Response_Type::RESPONSE_DEVICE_LIST)
        {
        }
        std::vector<DEV_PKG> devs;
    };

    struct RESPONSE_STATE_PKG : RESPONSE_PKG
    {
        RESPONSE_STATE_PKG() : RESPONSE_PKG(Response_Type::RESPONSE_STATE), state(false)
        {
        }
        QString name;
        bool state;
    };

    struct RESPONSE_WRITE_PKG : RESPONSE_PKG
    {
        RESPONSE_WRITE_PKG() : RESPONSE_PKG(Response_Type::RESPONSE_WRITE), channelNumber(0)
        {
        }
        QString name;
        uint8_t channelNumber;
    };

    struct RESPONSE_READ_PKG : RESPONSE_WRITE_PKG
    {
        RESPONSE_READ_PKG()
        {
            type = Response_Type::RESPONSE_READ;
        }
        QByteArray data;
    };

    static RESPONSE_PKG *accumResponsePacket(QByteArray &data);

private:
    static RESPONSE_PKG *createResponse_DEV_LIST_PKG(QByteArray &data);
    static DEV_PKG createResponse_DEV_PKG(QByteArray &data);

    static RESPONSE_PKG *createResponse_STATE_PKG(QByteArray &data);
    static RESPONSE_PKG *createResponse_READ_PKG(QByteArray &data);
    static RESPONSE_PKG *createResponse_WRITE_PKG(QByteArray &data);
};

/*
 * Генерация сырах данных клиентом для отправки серверу
 */

class Client_TX
{
    static QByteArray query_Device_List();
    static QByteArray query_STATE(const QString &deviceName);
    static QByteArray query_READ(const QString &deviceName, uint8_t channelNumber);
    static QByteArray query_WRITE(const QString &deviceName, uint8_t channelNumber, const QByteArray &rawData);
};
