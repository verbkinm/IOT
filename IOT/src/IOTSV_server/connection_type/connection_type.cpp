#include "connection_type.h"

Connection_type::Connection_type(const QString &name, Base_conn_type *parent) : Base_conn_type(name, parent)
{
    _conn_type = Conn_type::NONE;
}

qint64 Connection_type::write(const QByteArray &data)
{
    Q_UNUSED(data);
    return -1;
}

void Connection_type::connectToHost()
{

}

void Connection_type::disconnectFromHost()
{

}
