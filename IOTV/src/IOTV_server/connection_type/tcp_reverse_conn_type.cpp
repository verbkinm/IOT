#include "tcp_reverse_conn_type.h"

#include <QHostAddress>

TCP_REVERSE_conn_type::TCP_REVERSE_conn_type(const QString &name, QTcpSocket *socket, QObject *parent)
    : Base_conn_type{name, parent}, _tcpSocket(socket)
{
    _tcpSocket->setParent(this);
    _address = socket->peerAddress().toString();

    connect(socket, &QTcpSocket::readyRead, this, &TCP_REVERSE_conn_type::slotReadData);
    connect(socket, &QAbstractSocket::errorOccurred, this, &TCP_REVERSE_conn_type::slotError, Qt::QueuedConnection);
    connect(socket, &QAbstractSocket::stateChanged, this, &TCP_REVERSE_conn_type::slotSocketStateChanged, Qt::QueuedConnection);
    connect(socket,  &QTcpSocket::disconnected, this, &TCP_REVERSE_conn_type::slotSocketDisconnected);
}

TCP_REVERSE_conn_type::~TCP_REVERSE_conn_type()
{
    qDebug() << "destruct TCP_REVERSE_conn_type";
    _tcpSocket->abort();
}

qint64 TCP_REVERSE_conn_type::write(const QByteArray &data, qint64 size)
{
    if (_tcpSocket->state() != QAbstractSocket::ConnectedState)
        return 0;

    //    Log::write(_name +
    //                   ": data transmit to " +
    //                   _tcpSocket->peerAddress().toString() +
    //                   ":" +
    //                   QString::number(_tcpSocket->peerPort()) +
    //                   " -> " +
    //                   data.toHex(':'),
    //               Log::Write_Flag::FILE_STDOUT,
    //               ServerLog::DEFAULT_LOG_FILENAME);

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

void TCP_REVERSE_conn_type::slotSocketDisconnected()
{
    Log::write(CATEGORY::NET, _name + ": disconnected from " + _tcpSocket->peerAddress().toString()
                                  + ":" + QString::number(_tcpSocket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    emit signalDisconnected();
}

void TCP_REVERSE_conn_type::slotSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    QString strErr;
    switch (socketState)
    {
    case QAbstractSocket::UnconnectedState:
        strErr = "The socket is not connected.";
        break;
    case QAbstractSocket::HostLookupState:
        strErr = "The socket is performing a host name lookup.";
        break;
    case QAbstractSocket::ConnectingState:
        strErr = "The socket has started establishing a connection.";
        break;
    case QAbstractSocket::ConnectedState:
        strErr = "A connection is established.";
        break;
    case QAbstractSocket::BoundState:
        strErr = "The socket is bound to an address and port.";
        break;
    case QAbstractSocket::ClosingState:
        strErr = "The socket is about to close (data may still be waiting to be written).";
        break;
    case QAbstractSocket::ListeningState:
        strErr = "For internal use only).";
        break;
    }

    Log::write(CATEGORY::WARNING, _name + ": " + strErr,
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);
}

void TCP_REVERSE_conn_type::slotError(QAbstractSocket::SocketError error)
{
    QString strErr;
    switch (error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        strErr = "The connection was refused by the peer (or timed out).";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        strErr = "The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.";
        break;
    case QAbstractSocket::HostNotFoundError:
        strErr = "The host address was not found.";
        break;
    case QAbstractSocket::SocketAccessError:
        strErr = "The socket operation failed because the application lacked the required privileges.";
        break;
    case QAbstractSocket::SocketResourceError:
        strErr = "The local system ran out of resources (e.g., too many sockets).";
        break;
    case QAbstractSocket::SocketTimeoutError:
        strErr = "The socket operation timed out.";
        break;
    case QAbstractSocket::DatagramTooLargeError:
        strErr = "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).";
        break;
    case QAbstractSocket::NetworkError:
        strErr = "An error occurred with the network (e.g., the network cable was accidentally plugged out).";
        break;
    case QAbstractSocket::AddressInUseError:
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        break;
    case QAbstractSocket::ProxyNotFoundError:
        break;
    case QAbstractSocket::ProxyProtocolError:
        break;
    case QAbstractSocket::OperationError:
        break;
    case QAbstractSocket::SslInternalError:
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        break;
    case QAbstractSocket::TemporaryError:
        strErr = "A temporary error occurred (e.g., operation would block and socket is non-blocking).";
        break;
    case QAbstractSocket::UnknownSocketError:
        strErr = "UnknownSocketError";
        break;
    }

    Log::write(CATEGORY::ERROR,_name + ": " + strErr,
               Log::Write_Flag::FILE_STDERR,
               ServerLog::DEFAULT_LOG_FILENAME);
}
