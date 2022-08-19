#pragma once

#include <QByteArray>
#include <QString>

/*
 * Обработка сырах данных, полученных сервером от клиента
 */

class Server_RX
{
public:
    enum class Query_Type : uint8_t
    {
        QUERY_DEVICE_LIST,
        QUERY_STATE,
        QUERY_READ,
        QUERY_WRITE,
        QUERY_INCOMPLETE,
        QUERY_ERROR
    };

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
        bool state = false;
        QString name;

        friend bool operator==(const QUERY_READ_PKG &lhs, const QUERY_READ_PKG &rhs)
        {
            return std::make_tuple(lhs.type, lhs.state, lhs.name) ==
                    std::make_tuple(rhs.type, rhs.state, rhs.name);
        }
    };

    static QUERY_PKG *accumPacket(QByteArray &data);

//private:
//    static RESPONSE_PKG *createResponse_DEV_LIST_PKG(QByteArray &data);
//    static DEV_PKG createResponse_DEV_PKG(QByteArray &data);

//    static RESPONSE_PKG *createResponse_STATE_PKG(QByteArray &data);
//    static RESPONSE_PKG *createResponse_READ_PKG(QByteArray &data);
//    static RESPONSE_PKG *createResponse_WRITE_PKG(QByteArray &data);
};
