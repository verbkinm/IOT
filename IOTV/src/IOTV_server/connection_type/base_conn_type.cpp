#include "base_conn_type.h"

Base_conn_type::Base_conn_type(const QString& name, QObject *parent) : QObject(parent),
    _name(name), _address("none address")
{

}

Base_conn_type::~Base_conn_type()
{

}

Base_conn_type::Conn_type Base_conn_type::getConnectionType() const
{
    return _conn_type;
}

QString Base_conn_type::getAddress() const
{
    return _address;
}

void Base_conn_type::setAddress(const QString &address)
{
    _address = address;
}

qint64 Base_conn_type::write(const QByteArray &data)
{
    Q_UNUSED(data);
    return -1;
}

void Base_conn_type::connectToHost()
{

}

void Base_conn_type::disconnectFromHost()
{

}

QString Base_conn_type::ConnTypeToString(Base_conn_type::Conn_type conn_type)
{
    if(conn_type == Conn_type::NONE)
        return "NONE";
    else if(conn_type == Conn_type::COM)
        return "COM";
    else if(conn_type == Conn_type::ETHERNET)
        return "ETHERNET";

    return "UNKNOW";
}

QString Base_conn_type::getName() const
{
    return _name;
}
