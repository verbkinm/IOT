#include "tcp_conn_type.h"

TCP_conn_type::TCP_conn_type(const QString &name, const QString &address, quint16 port, Base_conn_type *parent) : Base_conn_type(name, parent),
    _tcpSocket(std::make_unique<QTcpSocket>()), _tcpPort(port)
{
    _address = address;
    _type = Conn_type::TCP;
    _tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(_tcpSocket.get(), &QAbstractSocket::connected, this, &TCP_conn_type::slotNewConnection);
    connect(_tcpSocket.get(), &QAbstractSocket::errorOccurred, this, &TCP_conn_type::slotError);
    connect(_tcpSocket.get(), &QAbstractSocket::stateChanged, this, &TCP_conn_type::slotSocketStateChanged);

    connect(&_reconnectTimer, &QTimer::timeout, this, &TCP_conn_type::connectToHost);
}

quint16 TCP_conn_type::getPort() const
{
    return _tcpPort;
}

void TCP_conn_type::setPort(quint16 port)
{
    _tcpPort = port;
}

qint64 TCP_conn_type::write(const QByteArray &data)
{
    Log::write(_name + ": data transmit to " + _tcpSocket->peerAddress().toString() +
               + ":" + QString::number(_tcpSocket->peerPort()) + " -> " + data.toHex(':'));

    return _tcpSocket->write(data);
}

void TCP_conn_type::connectToHost()
{
    _reconnectTimer.start(DEFAULT_INTERVAL);
    _tcpSocket->connectToHost(_address, _tcpPort, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
}

void TCP_conn_type::disconnectFromHost()
{
    _tcpSocket->disconnectFromHost();
}

QByteArray TCP_conn_type::readAll()
{
    return _tcpSocket->readAll();
}

void TCP_conn_type::slotNewConnection()
{
    _reconnectTimer.stop();
    Log::write(_name + ": connected to " + _tcpSocket->peerAddress().toString()
               + ":" + QString::number(_tcpSocket->peerPort()));

    connect(_tcpSocket.get(), &QTcpSocket::readyRead, this, &TCP_conn_type::slotReadData);
    connect(_tcpSocket.get(),  &QTcpSocket::disconnected, this, &TCP_conn_type::slotSocketDisconnected);

    emit signalConnected();
}

void TCP_conn_type::slotSocketDisconnected()
{
    Log::write(_name + ": disconnected from " + _tcpSocket->peerAddress().toString()
               + ":" + QString::number(_tcpSocket->peerPort()));

    disconnect(_tcpSocket.get(), &QTcpSocket::readyRead, this, &TCP_conn_type::slotReadData);
    disconnect(_tcpSocket.get(),  &QTcpSocket::disconnected, this, &TCP_conn_type::slotSocketDisconnected);

    emit signalDisconnected();
    _reconnectTimer.start(DEFAULT_INTERVAL);
}

void TCP_conn_type::slotSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    Q_UNUSED(socketState);
    return;
//    QAbstractSocket::UnconnectedState0The socket is not connected.
//    QAbstractSocket::HostLookupState1The socket is performing a host name lookup.
//    QAbstractSocket::ConnectingState2The socket has started establishing a connection.
//    QAbstractSocket::ConnectedState3A connection is established.
//    QAbstractSocket::BoundState4The socket is bound to an address and port.
//    QAbstractSocket::ClosingState6The socket is about to close (data may still be waiting to be written).
//    QAbstractSocket::ListeningState5For internal use only.
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

    Log::write("Error! " + _name + ": " + strErr);
}
