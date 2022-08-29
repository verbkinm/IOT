#pragma once

#include <QByteArray>
#include <QString>

#include "raw.h"

namespace IOTV_SC
{

enum : uint8_t
{
    QUERY_DEV_LIST_BYTE = 0x01,
    QUERY_STATE_FIRST_BYTE = 0x00,
    QUERY_STATE_SECOND_BYTE = 0x10,
    QUERY_READ_BYTE = 0x02,
    QUERY_WRITE_BYTE = 0x00
};

enum : uint8_t
{
    RESPONSE_DEV_LIST_BYTE = 0x05,
    RESPONSE_STATE_FIRST_BYTE = 0x04,
    RESPONSE_STATE_SECOND_BYTE = 0x10,
    RESPONSE_READ_BYTE = 0x06,
    RESPONSE_WRITE_BYTE = 0x04,

    STATE_BIT_MASK = 0x20
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

enum class Query_Type : uint8_t
{
    QUERY_DEVICE_LIST,
    QUERY_STATE,
    QUERY_READ,
    QUERY_WRITE,
    QUERY_INCOMPLETE,
    QUERY_ERROR
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

    friend bool operator==(const DEV_PKG &lhs, const DEV_PKG &rhs)
    {
        auto res = std::make_tuple(lhs.id, lhs.error, lhs.name, lhs.description, lhs.readChannel, lhs.writeChannel) ==
                std::make_tuple(rhs.id, rhs.error, rhs.name, rhs.description, rhs.readChannel, rhs.writeChannel);
        return res;
    }
};

struct RESPONSE_DEV_LIST_PKG : RESPONSE_PKG
{
    RESPONSE_DEV_LIST_PKG() : RESPONSE_PKG(Response_Type::RESPONSE_DEVICE_LIST)
    {
    }
    std::vector<DEV_PKG> devs;

    friend bool operator==(const RESPONSE_DEV_LIST_PKG &lhs, const RESPONSE_DEV_LIST_PKG &rhs)
    {
        return std::make_tuple(lhs.type, lhs.devs) ==
                std::make_tuple(rhs.type, rhs.devs);;
    }
};

struct RESPONSE_STATE_PKG : RESPONSE_PKG
{
    RESPONSE_STATE_PKG() : RESPONSE_PKG(Response_Type::RESPONSE_STATE), state(false)
    {
    }
    QString name;
    bool state;

    friend bool operator==(const RESPONSE_STATE_PKG &lhs, const RESPONSE_STATE_PKG &rhs)
    {
        return std::make_tuple(lhs.type, lhs.name, lhs.state) ==
                std::make_tuple(rhs.type, rhs.name, rhs.state);
    }
};

struct RESPONSE_WRITE_PKG : RESPONSE_PKG
{
    RESPONSE_WRITE_PKG() : RESPONSE_PKG(Response_Type::RESPONSE_WRITE), channelNumber(0)
    {
    }
    QString name;
    uint8_t channelNumber;

    friend bool operator==(const RESPONSE_WRITE_PKG &lhs, const RESPONSE_WRITE_PKG &rhs)
    {
        return std::make_tuple(lhs.type, lhs.name, lhs.channelNumber) ==
                std::make_tuple(rhs.type, rhs.name, rhs.channelNumber);
    }
};

struct RESPONSE_READ_PKG : RESPONSE_PKG
{
    RESPONSE_READ_PKG() : RESPONSE_PKG(Response_Type::RESPONSE_READ), channelNumber{0}
    {
    }
    QString name;
    uint8_t channelNumber;
    QByteArray data;

    friend bool operator==(const RESPONSE_READ_PKG &lhs, const RESPONSE_READ_PKG &rhs)
    {
        return std::make_tuple(lhs.type, lhs.name, lhs.channelNumber, lhs.data) ==
                std::make_tuple(rhs.type, rhs.name, rhs.channelNumber, rhs.data);
    }
};

/*
* Обработка сырах данных, полученных клиентом от сервера
*/

class Client_RX
{
public:
    static RESPONSE_PKG *accumPacket(QByteArray &data);

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
public:
    static QByteArray query_Device_List();
    static QByteArray query_STATE(const QString &deviceName);
    static QByteArray query_READ(const QString &deviceName, uint8_t channelNumber);
    static QByteArray query_WRITE(const QString &deviceName, uint8_t channelNumber, const QByteArray &rawData);
};

/*
* Обработка сырах данных, полученных сервером от клиента
*/

class Server_RX
{
public:
    struct QUERY_PKG
    {
        QUERY_PKG(Query_Type qType = Query_Type::QUERY_ERROR) : type(qType)
        {
        }
        Query_Type type;
    };

    struct QUERY_DEV_LIST_PKG : QUERY_PKG
    {
        QUERY_DEV_LIST_PKG() : QUERY_PKG(Query_Type::QUERY_DEVICE_LIST)
        {
        }

        friend bool operator==(const QUERY_DEV_LIST_PKG &lhs, const QUERY_DEV_LIST_PKG &rhs)
        {
            return std::make_tuple(lhs.type) == std::make_tuple(rhs.type);
        }
    };

    struct QUERY_STATE_PKG : QUERY_PKG
    {
        QUERY_STATE_PKG() : QUERY_PKG(Query_Type::QUERY_STATE)
        {
        }
        bool state = false;
        QString name;

        friend bool operator==(const QUERY_STATE_PKG &lhs, const QUERY_STATE_PKG &rhs)
        {
            return std::make_tuple(lhs.type, lhs.state, lhs.name) ==
                    std::make_tuple(rhs.type, rhs.state, rhs.name);
        }
    };

    struct QUERY_READ_PKG : QUERY_PKG
    {
        QUERY_READ_PKG() : QUERY_PKG(Query_Type::QUERY_READ)
        {
        }
        QString name;
        uint8_t channelNumber = 0;

        friend bool operator==(const QUERY_READ_PKG &lhs, const QUERY_READ_PKG &rhs)
        {
            return std::make_tuple(lhs.type, lhs.name, lhs.channelNumber) ==
                    std::make_tuple(rhs.type, rhs.name, rhs.channelNumber);
        }
    };

    struct QUERY_WRITE_PKG : QUERY_PKG
    {
        QUERY_WRITE_PKG() : QUERY_PKG(Query_Type::QUERY_WRITE)
        {
        }
        QString name;
        uint8_t channelNumber = 0;
        QByteArray data;

        friend bool operator==(const QUERY_WRITE_PKG &lhs, const QUERY_WRITE_PKG &rhs)
        {
            return std::make_tuple(lhs.type, lhs.name, lhs.channelNumber, lhs.data) ==
                    std::make_tuple(rhs.type, rhs.name, rhs.channelNumber, rhs.data);
        }
    };

    static QUERY_PKG *accumPacket(QByteArray &data);

private:
    static QUERY_PKG *createQuery_DEV_LIST_PKG(QByteArray &data);
    static QUERY_PKG *createQuery_READ_PKG(QByteArray &data);
    static QUERY_PKG *createQuery_STATE_PKG(QByteArray &data);
    static QUERY_PKG *createQuery_WRITE_PKG(QByteArray &data);
};

/*
* Генерация сырах данных сервером для отправки клиенту
*/

class Server_TX
{
public:
    static QByteArray response_DEV_LIST(const RESPONSE_DEV_LIST_PKG &pkg);
    static QByteArray response_STATE(const RESPONSE_STATE_PKG &pkg);
    static QByteArray response_READ(const RESPONSE_READ_PKG &pkg);
    static QByteArray response_WRITE(const RESPONSE_WRITE_PKG &pkg);
};

}
