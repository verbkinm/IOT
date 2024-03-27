#include "udp_conn_type.h"

Udp_conn_type::Udp_conn_type(const QString &name, const QString &address, quint16 port, QObject *parent)
    : Base_conn_type(name, parent),
    _udpSocket(new QUdpSocket(this))
{
    _address = address;
    _udpPort = port;
    _type = Conn_type::UDP;


    connect(_udpSocket, &QAbstractSocket::stateChanged, this, &Udp_conn_type::slotSocketStateChange, Qt::DirectConnection);
    connect(_udpSocket, &QAbstractSocket::errorOccurred, this, &Udp_conn_type::slotError);
    connect(_udpSocket, &QAbstractSocket::readyRead, this, &Udp_conn_type::slotReadData, Qt::DirectConnection);

    if (!_udpSocket->bind(QHostAddress::AnyIPv4))
    {
        Log::write(_name + ": Error bind address",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        qDebug() << this;
    }
    else
    {
        Log::write(_name + ": Bind address " + _udpSocket->localAddress().toString() + ":" + QString::number(_udpSocket->localPort()),
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
    }
}

Udp_conn_type::~Udp_conn_type()
{
    _udpSocket->abort();
//    qDebug() << "udp conn type destruct";
}

qint64 Udp_conn_type::write(const QByteArray &data, qint64 size)
{
    //    Log::write(_name +
    //               ": data transmit to " +
    //               _address +
    //               ":" +
    //               QString::number(_udpPort) +
    //               " -> " +
    //               data.toHex(':'),
    //               Log::Write_Flag::FILE_STDOUT,
    //               ServerLog::DEFAULT_LOG_FILENAME);

    if (size == -1)
        return _udpSocket->writeDatagram(data, QHostAddress(_address), _udpPort);

    return _udpSocket->writeDatagram(data.data(), size, QHostAddress(_address), _udpPort);
}

void Udp_conn_type::connectToHost()
{
    emit signalConnected();
}

void Udp_conn_type::disconnectFromHost()
{
    emit signalDisconnected();
}

QByteArray Udp_conn_type::readAll()
{
    QByteArray data;

    QNetworkDatagram datagram = _udpSocket->receiveDatagram();
    data.append(datagram.data());

    if (datagram.isValid())
        data.append(datagram.data());

    return data;
}

void Udp_conn_type::slotError(QAbstractSocket::SocketError error)
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

void Udp_conn_type::slotSocketStateChange(QAbstractSocket::SocketState socketState)
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
