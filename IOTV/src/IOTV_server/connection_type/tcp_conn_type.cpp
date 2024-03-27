#include "tcp_conn_type.h"

TCP_conn_type::TCP_conn_type(const QString &name, const QString &address, quint16 port, QObject *parent) :
    Base_conn_type(name, parent),
    _tcpSocket(new QTcpSocket(this))
{
    _tcpPort = port;
    _address = address;
    _type = Conn_type::TCP;

    _tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(_tcpSocket, &QAbstractSocket::connected, this, &TCP_conn_type::slotNewConnection, Qt::QueuedConnection);
    connect(_tcpSocket, &QAbstractSocket::errorOccurred, this, &TCP_conn_type::slotError, Qt::QueuedConnection);
    connect(_tcpSocket, &QAbstractSocket::stateChanged, this, &TCP_conn_type::slotSocketStateChanged, Qt::QueuedConnection);
}

TCP_conn_type::~TCP_conn_type()
{
    _tcpSocket->abort();
//    qDebug() << "destruct TCP_conn_type";
}

qint64 TCP_conn_type::write(const QByteArray &data, qint64 size)
{
    if (_tcpSocket->state() != QAbstractSocket::ConnectedState)
        return 0;

//    Log::write(_name +
//               ": data transmit to " +
//               _tcpSocket.peerAddress().toString() +
//               ":" +
//               QString::number(_tcpSocket.peerPort()) +
//               " -> " +
//               data.toHex(':'),
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

void TCP_conn_type::connectToHost()
{
    disconnectFromHost();
    Log::write(_name + ": try to connect...", Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);
    _tcpSocket->connectToHost(_address, _tcpPort, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
}

void TCP_conn_type::disconnectFromHost()
{
    _tcpSocket->abort();
}

QByteArray TCP_conn_type::readAll()
{
    return _tcpSocket->readAll();
}

void TCP_conn_type::slotNewConnection()
{
    Log::write(_name + ": connected to " + _tcpSocket->peerAddress().toString()
                   + ":" + QString::number(_tcpSocket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    connect(_tcpSocket, &QTcpSocket::readyRead, this, &TCP_conn_type::slotReadData);
    connect(_tcpSocket,  &QTcpSocket::disconnected, this, &TCP_conn_type::slotSocketDisconnected);

    emit signalConnected();
}

void TCP_conn_type::slotSocketDisconnected()
{
    expectedDataSize = 0;

    Log::write(_name + ": disconnected from " + _tcpSocket->peerAddress().toString()
                   + ":" + QString::number(_tcpSocket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    disconnect(_tcpSocket, &QTcpSocket::readyRead, this, &TCP_conn_type::slotReadData);
    disconnect(_tcpSocket,  &QTcpSocket::disconnected, this, &TCP_conn_type::slotSocketDisconnected);

    emit signalDisconnected();
}

void TCP_conn_type::slotSocketStateChanged(QAbstractSocket::SocketState socketState)
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

    Log::write(_name + ": " + strErr,
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);
}

void TCP_conn_type::slotError(QAbstractSocket::SocketError error)
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

    Log::write(_name + ": " + strErr,
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);
}
