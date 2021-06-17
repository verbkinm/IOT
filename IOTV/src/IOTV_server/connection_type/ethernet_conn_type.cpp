#include "ethernet_conn_type.h"

Ethernet_conn_type::Ethernet_conn_type(const QString &name, const QString &address,
                                       quint16 port, Base_conn_type *parent) : Base_conn_type(name, parent),
    _tcpSocket(std::make_unique<QTcpSocket>()), _tcpPort(port)
{
    _address = address;
    _conn_type = Conn_type::ETHERNET;

    connect(_tcpSocket.get(), &QAbstractSocket::connected, this, &Ethernet_conn_type::slotNewConnection);
    connect(_tcpSocket.get(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

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
    QString strOut = _name + ": data transmit to " + _tcpSocket->peerAddress().toString() +
                     + ":" + QString::number(_tcpSocket->peerPort()) + " -> " + data.toHex(':');
    Log::write(strOut);
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

void Ethernet_conn_type::slotNewConnection()
{
    QString strOut = _name + ": connected to " + _tcpSocket->peerAddress().toString()
                     + ":" + QString::number(_tcpSocket->peerPort());
    Log::write(strOut);

    connect(_tcpSocket.get(), &QTcpSocket::readyRead, this, &Ethernet_conn_type::slotReadData);
    connect(_tcpSocket.get(),  &QTcpSocket::disconnected, this, &Ethernet_conn_type::slotSocketDisconnected);

    _reconnectTimer.stop();
    emit signalConnected();
}

void Ethernet_conn_type::slotSocketDisconnected()
{
    QString strOut = _name + ": disconnected from " + _tcpSocket->peerAddress().toString()
                   + ":" + QString::number(_tcpSocket->peerPort());
    Log::write(strOut);

    disconnect(_tcpSocket.get(), &QTcpSocket::readyRead, this, &Ethernet_conn_type::slotReadData);
    disconnect(_tcpSocket.get(),  &QTcpSocket::disconnected, this, &Ethernet_conn_type::slotSocketDisconnected);

    emit signalDisconnected();
    _reconnectTimer.start(DEFAULT_INTERVAL);
}

void Ethernet_conn_type::slotReadData()
{
    QByteArray data = _tcpSocket->readAll();

    QString strOut = _name + ": data riceved from " + _tcpSocket->peerAddress().toString()
                     + ":" + QString::number(_tcpSocket->peerPort()) + " <- " + data.toHex(':');
    Log::write(strOut);
    emit signalDataRiceved(data);
}

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
        break;
    case QAbstractSocket::SocketAccessError:
        break;
    case QAbstractSocket::SocketResourceError:
        break;
    case QAbstractSocket::SocketTimeoutError:
        break;
    case QAbstractSocket::DatagramTooLargeError:
        break;
    case QAbstractSocket::NetworkError:
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
        break;
    case QAbstractSocket::UnknownSocketError:
        strErr = "UnknownSocketError";
        break;
    }

    Log::write(_name + ": " + strErr);
}
