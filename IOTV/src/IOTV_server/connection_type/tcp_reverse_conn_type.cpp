#include "tcp_reverse_conn_type.h"

TCP_REVERSE_conn_type::TCP_REVERSE_conn_type(const QString &name, QTcpSocket *socket, QObject *parent)
    : Base_conn_type{name, parent}, _tcpSocket(socket)
{
    connect(socket, &QTcpSocket::readyRead, this, &TCP_REVERSE_conn_type::slotReadData);
}

qint64 TCP_REVERSE_conn_type::write(const QByteArray &data, qint64 size)
{
    if (_tcpSocket->state() != QAbstractSocket::ConnectedState)
        return 0;

    Log::write(_name +
                   ": data transmit to " +
                   _tcpSocket->peerAddress().toString() +
                   ":" +
                   QString::number(_tcpSocket->peerPort()) +
                   " -> " +
                   data.toHex(':'),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    if (size == -1)
    {
        auto s = _tcpSocket->write(data);
        _tcpSocket->flush();
        return s;
    }

    auto s = _tcpSocket->write(data.data(), size);
    _tcpSocket->flush();
    return s;
}

void TCP_REVERSE_conn_type::connectToHost()
{
    emit signalConnected();
}

void TCP_REVERSE_conn_type::disconnectFromHost()
{
    _tcpSocket->abort();
}

QByteArray TCP_REVERSE_conn_type::readAll()
{
    return _tcpSocket->readAll();
}
