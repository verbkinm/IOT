#include "network_module.h"

Network_Module::Network_Module()
{
    connect(this, &QTcpServer::newConnection, this, &Network_Module::slotNewConnection);
}

void Network_Module::slotNewConnection()
{
    QTcpSocket* socket = this->nextPendingConnection();
    Log::write("new connection: "+ socket->peerAddress().toString() + QString::number(socket->peerPort()));
    connect(socket, &QTcpSocket::readyRead, this, &Network_Module::slotDataRecived);
    connect(socket, &QTcpSocket::disconnected, this, &Network_Module::slotDisconnected);
}

void Network_Module::slotDataRecived()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = socket->readAll();

    Log::write("data recived form " + socket->peerAddress().toString() + "." + QString::number(socket->peerPort())
               + " <- " + data.toHex(':'));

    Protocol_class::query_type dataType = Protocol_class::checkQueryData(data.data());

    if(dataType == Protocol_class::query_type::QUERY_WAY)
    {
        data.clear();
        uint8_t byte = 0x05;
        data.append(byte);

        data.append(iotServer._id);

        byte = iotServer._description.size() >> 8;
        data.append(byte);
        byte = iotServer._description.size();
        data.append(byte);

        byte = iotServer._userDescription.size() >> 8;
        data.append(byte);
        byte = iotServer._userDescription.size();
        data.append(byte);

        byte = READ_CHANNEL_LENGTH << 4;
        byte |= WRITE_CHANNEL_LENGTH;
        data.append(byte);

        data.append(iotServer._description.c_str());
        data.append(iotServer._userDescription.c_str());

        for (uint8_t i = 0; i < READ_CHANNEL_LENGTH; i++)
            data.append(Raw::toUInt8(iotServer._readChannelType[i]));

        for (uint8_t i = 0; i < WRITE_CHANNEL_LENGTH; i++)
            data.append(Raw::toUInt8(iotServer._writeChannelType[i]));
    }
    else if(dataType == Protocol_class::query_type::QUERY_READ)
    {
        uint8_t chNumber = data.at(0) >> 4;
        data.clear();

        data.append((chNumber << 4) | 6 );
        if(chNumber >= READ_CHANNEL_LENGTH)
        {
            data.append('\0');
            socket->write(data);
            return;
        }

        Raw::RAW raw = iotServer._readChannel[chNumber];
        for (uint8_t i = 0; i < 8; i++)
        {
            data.append(raw.array[i]);
        }
    }
    else if(dataType == Protocol_class::query_type::QUERY_WRITE)
    {
        uint8_t chNumber = data.at(0) >> 4;
        data.clear();

        data.append((chNumber << 4) | 4 );
    }

    socket->write(data);

    Log::write("data transmit to " + socket->peerAddress().toString() + "." + QString::number(socket->peerPort())
               + " -> " + data.toHex(':'));
}

void Network_Module::slotDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    disconnect(socket, &QTcpSocket::readyRead, this, &Network_Module::slotDataRecived);
    disconnect(socket,  &QTcpSocket::disconnected, this, &Network_Module::slotDisconnected);

    QString strOut = "disconnected from " + socket->peerAddress().toString()
                   + "." + QString::number(socket->peerPort());
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
}
