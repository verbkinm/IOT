#include "network_module.h"

Network_Module::Network_Module()
{
    connect(this, &QTcpServer::newConnection, this, &Network_Module::slotNewConnection);
}

void Network_Module::response(QTcpSocket* socket, QByteArray &data)
{
    std::vector<uint8_t> vectorData;
    for (auto byte : data)
        vectorData.push_back(byte);

    Protocol_class::query_type dataType = Protocol_class::checkQueryData(vectorData);

    if(dataType == Protocol_class::query_type::QUERY_WAY)
        Protocol_class::response_WAY(iotServer, vectorData);
    else if(dataType == Protocol_class::query_type::QUERY_READ)
        Protocol_class::response_READ(iotServer, vectorData);
    else if(dataType == Protocol_class::query_type::QUERY_WRITE)
        Protocol_class::response_WRITE(iotServer, vectorData);
    else if(dataType == Protocol_class::query_type::QUERY_PING)
        Protocol_class::response_Pong(vectorData);

    QByteArray sentData;
    for (auto byte : vectorData)
        sentData.append(byte);

    socket->write(sentData);

    Log::write("data transmit to " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort())
               + " -> " + sentData.toHex(':'));

    postResponse(socket, data);
}

void Network_Module::postResponse(QTcpSocket *socket, QByteArray &data)
{
    std::vector<uint8_t> vectorData;
    for (auto byte : data)
        vectorData.push_back(byte);

    Protocol_class::query_type dataType = Protocol_class::checkQueryData(vectorData);

    if(dataType == Protocol_class::query_type::QUERY_WAY)
        data.remove(0, 1);
    else if(dataType == Protocol_class::query_type::QUERY_READ)
        data.remove(0, 1);
    else if(dataType == Protocol_class::query_type::QUERY_PING)
        data.remove(0, 1);
    else if(dataType == Protocol_class::query_type::QUERY_WRITE)
        data.remove(0, (data[1] | data[2]) + 3);

    if(data.length())
        response(socket, data);
}

void Network_Module::slotNewConnection()
{
    QTcpSocket* socket = this->nextPendingConnection();
    Log::write("new connection: "+ socket->peerAddress().toString() + QString::number(socket->peerPort()));

    connect(socket, &QTcpSocket::readyRead, this, &Network_Module::slotDataRecived);
    connect(socket, &QTcpSocket::disconnected, this, &Network_Module::slotDisconnected);
    connect(socket, &QTcpSocket::disconnected, this, &QObject::deleteLater);
}

void Network_Module::slotDataRecived()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = socket->readAll();

    Log::write("data recived form " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort())
               + " <- " + data.toHex(':'));
    response(socket, data);
}

void Network_Module::slotDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    disconnect(socket, &QTcpSocket::readyRead, this, &Network_Module::slotDataRecived);
    disconnect(socket,  &QTcpSocket::disconnected, this, &Network_Module::slotDisconnected);

    QString strOut = "disconnected from " + socket->peerAddress().toString()
            + ":" + QString::number(socket->peerPort());
    Log::write(strOut);
}

void Network_Module::slotError(QAbstractSocket::SocketError error)
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

    Log::write(this->objectName() + ": " + strErr);

    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}
