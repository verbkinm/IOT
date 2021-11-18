#include "server.h"

static int default_interval = 5000;
static int reconnecting_count =  5;
static int number = 0;

Server::Server(QObject *parent) : QObject(parent), _name("server" + QString::number(number++)),
    _serverAddress("127.0.0.1"),
    _serverPort(2022),
    _reconnectTimerTrying(0)
{
    newObjectName();

    connect(&_socket, &QAbstractSocket::connected, this, &Server::slotConnected);
    connect(&_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    connect(&_reconnectTimer, &QTimer::timeout, this, &Server::connectToHost);
}

Server::~Server()
{
    disconnect(&_socket,  &QTcpSocket::disconnected, this, &Server::slotDisconnected);
    _socket.disconnectFromHost();
    destroyObservers();
}

QString Server::getName() const
{
    return _name;
}

void Server::setName(const QString &name)
{
    _name = name;
    newObjectName();
    notify();
}

QString Server::getServerAddress() const
{
    return _serverAddress;
}

void Server::setServerAddres(const QString &serverAddress)
{
    _serverAddress = serverAddress;
    newObjectName();
    notify();
}

quint16 Server::getServerPort() const
{
    return _serverPort;
}

void Server::setServerPort(const quint16 &serverPort)
{
    _serverPort = serverPort;
    newObjectName();
    notify();
}

void Server::setServerNAP(const QString &name, const QString &serverAddress, const quint16 &serverPort)
{
    _name = name;
    _serverAddress = serverAddress;
    _serverPort = serverPort;

    newObjectName();
    notify();
}

QAbstractSocket::SocketState Server::state() const
{
    return _socket.state();
}

quint16 Server::getDevicesCount() const
{
    return _devices.size();
}

quint16 Server::getDeviceOnline() const
{
    int count = 0;
    for (auto &elem : _devices)
    {
        if(elem.second->getState())
            count++;
    }

    return count;
}

void Server::connectToHost()
{
    if(_reconnectTimerTrying > reconnecting_count)
    {
        _reconnectTimer.stop();
        _reconnectTimerTrying = 0;
        notify();
        return;
    }

    _reconnectTimer.start(default_interval);
    _reconnectTimerTrying += 1;

    _socket.connectToHost(_serverAddress, _serverPort, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
}

void Server::disconnectFromHost()
{
    _socket.disconnectFromHost();
    emit signalDisconnected();
}

qint64 Server::writeData(QByteArray &data)
{
    return _socket.write(data);
}

void Server::createDevice(const QByteArray &data, const QString &name)
{
    _devices[name] = std::make_shared<Device>(*this, name, data.at(1), this);
    IOTV_SH::response_WAY(*_devices[name], data);

    const auto it = _alias.find(name);
    if(it != _alias.cend())
        _devices[name]->setViewName(_alias.at(name));
}

void Server::newObjectName()
{
    this->setObjectName(_name + "(" + _serverAddress + ":" + QString::number(_serverPort) + ")");
}

const std::map<QString, std::shared_ptr<Device> > &Server::getDevices() const
{
    return _devices;
}

void Server::addAlias(const QString &name, const QString &aliasName)
{
    if(name.length())
        _alias[name] = aliasName;
}

const std::map<QString, QString> &Server::getAlias() const
{
    return _alias;
}

QList<QObject*> Server::getDevicesToQML()
{
    QList<QObject*> result;
    for (auto &[key, value] : _devices)
    {
        result.append(value.get());
    }

    return result;
}

void Server::slotConnected()
{
    emit signalConnected();

    QString strOut = _name + ": connected to " + _socket.peerAddress().toString()
            + ":" + QString::number(_socket.peerPort());
    Log::write(strOut);

    connect(&_socket, &QTcpSocket::readyRead, this, &Server::slotReadData);
    connect(&_socket,  &QTcpSocket::disconnected, this, &Server::slotDisconnected);

    _reconnectTimer.stop();

    QByteArray data;
    IOTV_SC::query_Device_List(data);
    Log::write("Data send to " + _socket.peerAddress().toString() + ":"
               + QString::number(_socket.peerPort())
               + " -> " + data.toHex(':'));
    _socket.write(data);

    notify();
}

void Server::slotDisconnected()
{
    QString strOut = _name + ": disconnected from " + _socket.peerAddress().toString()
            + ":" + QString::number(_socket.peerPort());
    Log::write(strOut);

    disconnect(&_socket, &QTcpSocket::readyRead, this, &Server::slotReadData);
    disconnect(&_socket,  &QTcpSocket::disconnected, this, &Server::slotDisconnected);

    _devices.clear();

    notify();

    emit signalDisconnected();
}

void Server::slotReadData()
{
    static QByteArray data;
    data += _socket.readAll();

    while(data.size())
    {
        std::pair<bool, int> accumPacketResponse = IOTV_SC::accumResponcePacket(data);

        if(!accumPacketResponse.first)
        {
            data.clear();
            return;
        }
        if(accumPacketResponse.first && accumPacketResponse.second)
        {
            QByteArray packetData = data.mid(0, accumPacketResponse.second);
            Log::write("Data recived form " + _socket.peerAddress().toString() + ":"
                       + QString::number(_socket.peerPort())
                       + " <- " + data.toHex(':'));

            IOTV_SC::Response_Type dataType = IOTV_SC::checkResponsetData(packetData);

            if(dataType == IOTV_SC::Response_Type::RESPONSE_DEVICE_LIST)
            {
                packetData = packetData.mid(2);
                while (packetData.size())
                {
                    uint8_t nameLength = packetData.at(0) >> 3;
                    QString name = packetData.mid(1, nameLength);
                    packetData = packetData.mid(nameLength);
                    packetData[0] = 0x05;

                    std::pair<bool, int> WAY_Packet = IOTV_SH::accumPacket(packetData);
                    if(!WAY_Packet.first)
                    {
                        data.clear();
                        return;
                    }
                    if(WAY_Packet.first && WAY_Packet.second)
                    {
                        QByteArray buf = packetData.mid(0, WAY_Packet.second);
                        createDevice(buf, name);
                        packetData = packetData.mid(WAY_Packet.second);
                    }
                }
                emit signalDevicesCreated();
            }
            else
            {
                QString deviceName;
                if(IOTV_SC::responseName(packetData, deviceName))
                {
                    if(_devices.find(deviceName) != _devices.end())
                        _devices.at(deviceName)->dataResived(packetData);
                    else
                        Log::write(_name + ": " + "Recived data to unknow device name - " + deviceName, Log::Flags::WRITE_TO_FILE_AND_STDERR);
                }
            }
            data = data.mid(accumPacketResponse.second);
        }
        else
            break;

        notify();
        emit totalDeviceChanged(); //!!!
    }
}

void Server::slotError(QAbstractSocket::SocketError error)
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
}
