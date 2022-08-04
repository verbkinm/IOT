#include "base_conn_type.h"

Base_conn_type::Base_conn_type(const QString& name, QObject *parent) : QObject(parent),
    _name(name), _address("none address")
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
    else if(conn_type == Conn_type::TCP)
        return "TCP";
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
    _host_buffer_data += readAll();

    if (_host_buffer_data.size() > BUFFER_MAX_SIZE)
    {
        _host_buffer_data.clear();
        return;
    }

    QByteArray tmp = _host_buffer_data;
    IOTV_SH::RESPONSE_PKG accumPacketResponse = IOTV_SH::accumPacket(_host_buffer_data);

    if (accumPacketResponse.type != IOTV_SH::Response_Type::ERROR)
    {
        int difference = tmp.size() - _host_buffer_data.size();
        QString strOut = _name + ": data riceved from " + _address + " <- " + tmp.mid(difference).toHex(':');
        Log::write(strOut);

        emit signalDataRiceved(accumPacketResponse);
    }
}

QString Base_conn_type::getName() const
{
    return _name;
}
