#include "udp_conn_type.h"

Udp_conn_type::Udp_conn_type(const QString &name, const QString &address, quint16 port, QObject *parent)
    : Base_conn_type(name, parent)
{
    Q_UNUSED(port);
    _address = address;
    _type = Conn_type::UDP;
}

qint64 Udp_conn_type::write(const QByteArray &data, qint64 size)
{
    Q_UNUSED(size);
    Q_UNUSED(data);
    return -1;
}

void Udp_conn_type::connectToHost()
{

}

void Udp_conn_type::disconnectFromHost()
{

}
