#include "ethernet_conn_type.h"

Ethernet_conn_type::Ethernet_conn_type(const QString &name, const QString &address,
                                       quint16 port, Base_conn_type *parent) : Base_conn_type(name, parent),
    _tcpSocket(std::make_unique<QTcpSocket>()), _tcpPort(port)
{
    _address = address;
    _type = Conn_type::ETHERNET;

    connect(_tcpSocket.get(), &QAbstractSocket::connected, this, &Ethernet_conn_type::slotNewConnection);
    connect(_tcpSocket.get(), &QAbstractSocket::errorOccurred, this, &Ethernet_conn_type::slotError);

    connect(&_reconnectTimer, &QTimer::timeout, this, &Ethernet_conn_type::connectToHost);
}

quint16 Ethernet_conn_type::getPort() const
{
    return _tcpPort;
}

void Ethernet_conn_type::setPort(quint16 port)
{
    _tcpPort = port;
}

qint64 Ethernet_conn_type::write(const QByteArray &data)
{
    Log::write(_name + ": data transmit to " + _tcpSocket->peerAddress().toString() +
               + ":" + QString::number(_tcpSocket->peerPort()) + " -> " + data.toHex(':'));
    return _tcpSocket->write(data);
}

void Ethernet_conn_type::connectToHost()
{
    _reconnectTimer.start(DEFAULT_INTERVAL);
    _tcpSocket->connectToHost(_address, _tcpPort, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
}

void Ethernet_conn_type::disconnectFromHost()
{
    _tcpSocket->disconnectFromHost();
}

QByteArray Ethernet_conn_type::readAll()
{
    return _tcpSocket->readAll();
}

void Ethernet_conn_type::slotNewConnection()
{
    _reconnectTimer.stop();
    Log::write(_name + ": connected to " + _tcpSocket->peerAddress().toString()
               + ":" + QString::number(_tcpSocket->peerPort()));

    connect(_tcpSocket.get(), &QTcpSocket::readyRead, this, &Ethernet_conn_type::slotReadData);
    connect(_tcpSocket.get(),  &QTcpSocket::disconnected, this, &Ethernet_conn_type::slotSocketDisconnected);

    emit signalConnected();
}

void Ethernet_conn_type::slotSocketDisconnected()
{
    Log::write(_name + ": disconnected from " + _tcpSocket->peerAddress().toString()
               + ":" + QString::number(_tcpSocket->peerPort()));

    disconnect(_tcpSocket.get(), &QTcpSocket::readyRead, this, &Ethernet_conn_type::slotReadData);
    disconnect(_tcpSocket.get(),  &QTcpSocket::disconnected, this, &Ethernet_conn_type::slotSocketDisconnected);

    emit signalDisconnected();
    _reconnectTimer.start(DEFAULT_INTERVAL);
}

//void Ethernet_conn_type::slotReadData()
//{
//    static QByteArray data;
//    data += _tcpSocket->readAll();

//    std::pair<bool, int> accumPacketResponse = IOTV_SH::accumPacket(data);

//    if(!accumPacketResponse.first)
//    {
//        data.clear();
//        return;
//    }
//    if(accumPacketResponse.first && accumPacketResponse.second > 0)
//    {
//        QByteArray buffer = data.mid(0, accumPacketResponse.second);
//        QString strOut = _name + ": data riceved from " + _address + " <- " + buffer.toHex(':');
//        Log::write(strOut);

//        emit signalDataRiceved(buffer);
//        data = data.mid(accumPacketResponse.second);
//    }

////    static QByteArray data;
////    QByteArray buffer;

////    data += _tcpSocket->readAll();

////    if(!data.contains("\r\n"))
////    {
////        if(data.length() > BUFFER_MAX_SIZE)
////            Log::write(_name + "Buffer overload: " + data, Log::Flags::WRITE_TO_FILE_AND_STDERR);

////        return;
////    }
////    else
////    {
////        auto index = data.indexOf("\r\n");
////        buffer = data.mid(0, index);
////        data = data.mid(index + 2);

////        if(!buffer.size() || buffer == "\r\n")
////            return;

////        QString strOut = _name + ": data riceved from " + _tcpSocket->peerAddress().toString()
////                         + ":" + QString::number(_tcpSocket->peerPort()) + " <- " + buffer.toHex(':');
////        Log::write(strOut);
////        emit signalDataRiceved(buffer);
////    }
//}

void Ethernet_conn_type::slotError(QAbstractSocket::SocketError error)
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

    Log::write(_name + ": " + strErr);
}
