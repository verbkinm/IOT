#include "base_conn_type.h"
#include <QDebug>

Base_conn_type::Base_conn_type(const QString& name, QObject *parent) : QObject(parent),
    expectedDataSize(0),
    _name(name), _address("none address"), _type(Conn_type::NONE)
{

}

Base_conn_type::Conn_type Base_conn_type::getConnectionType() const
{
    return _type;
}

QString Base_conn_type::getAddress() const
{
    return _address;
}

void Base_conn_type::setAddress(const QString &address)
{
    _address = address;
}

QString Base_conn_type::ConnTypeToString(Base_conn_type::Conn_type conn_type)
{
    switch (conn_type)
    {
    case Conn_type::NONE:
        return "NONE";
        break;
    case Conn_type::COM:
        return "COM";
        break;
    case Conn_type::TCP:
        return "TCP";
        break;
    case Conn_type::TCP_REVERSE:
        return "TCP_REVERSE";
        break;
    case Conn_type::UDP:
        return "UDP";
        break;
    case Conn_type::FILE:
        return "FILE";
        break;
    default:
        return "UNKNOW";
        break;
    }
}

bool Base_conn_type::isIpConnectionType(const QString &conn_type)
{
    if (conn_type == connectionType::TCP || conn_type == connectionType::UDP || conn_type == connectionType::TCP_REVERSE)
        return true;

    return false;
}

void Base_conn_type::setName(const QString &newName)
{
    _name = newName;
}

QByteArray Base_conn_type::readAll()
{
    return {0};
}

void Base_conn_type::slotReadData()
{
    QByteArray inData = readAll();

//    Log::write(_name + ": data response <- " + inData. toHex(':'),
//               Log::Write_Flag::FILE_STDOUT,
//               ServerLog::DEFAULT_LOG_FILENAME);

    emit signalDataRiceved(inData);
}

QString Base_conn_type::getName() const
{
    return _name;
}
