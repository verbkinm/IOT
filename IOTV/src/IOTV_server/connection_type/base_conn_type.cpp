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
    else if(conn_type == Conn_type::FILE)
        return "FILE";

    return "UNKNOW";
}

QByteArray Base_conn_type::readAll()
{
    return {0};
}

void Base_conn_type::slotReadData()
{
    static QByteArray data;
    data += readAll();

    std::pair<bool, int> accumPacketResponse = IOTV_SH::accumPacket(data);

    if(!accumPacketResponse.first)
    {
        data.clear();
        return;
    }
    if(accumPacketResponse.first && accumPacketResponse.second > 0)
    {
        QByteArray buffer = data.mid(0, accumPacketResponse.second);
        QString strOut = _name + ": data riceved from " + _address + " <- " + buffer.toHex(':');
        Log::write(strOut);

        emit signalDataRiceved(buffer);
        data = data.mid(accumPacketResponse.second);
    }
}

QString Base_conn_type::getName() const
{
    return _name;
}
