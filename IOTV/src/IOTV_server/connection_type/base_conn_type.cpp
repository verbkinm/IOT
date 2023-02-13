#include "base_conn_type.h"
#include <QDebug>

Base_conn_type::Base_conn_type(const QString& name, QObject *parent) : QObject(parent),
    expectedDataSize(0),
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

qint64 Base_conn_type::write(const QByteArray &data, qint64 size)
{
    Q_UNUSED(data);
    Q_UNUSED(size)

    return -1;
}

void Base_conn_type::clearDataBuffer()
{
    std::lock_guard lg(_hostBuffMutex);
    _host_buffer_data.clear();
}

void Base_conn_type::setDataBuffer(QByteArray &data)
{
    std::lock_guard lg(_hostBuffMutex);
    _host_buffer_data = data;
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
    std::lock_guard lg(_hostBuffMutex);

    _host_buffer_data += readAll();
    Log::write(_name + ": data response <- " + _host_buffer_data.toHex(':'),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG);

    if (_host_buffer_data.size() >= BUFFER_MAX_SIZE)
    {
        _host_buffer_data.clear();
        Log::write(_name + ": buffer cleared!",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG);
        return;
    }

    emit signalDataRiceved(_host_buffer_data);
}

QString Base_conn_type::getName() const
{
    return _name;
}
