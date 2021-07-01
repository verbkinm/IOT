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
    _socket.disconnected();
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

uint16_t Server::getServerPort() const
{
    return _serverPort;
}

void Server::setServerPort(const uint16_t &serverPort)
{
    _serverPort = serverPort;
    newObjectName();
    notify();
}

void Server::setServerNAP(const QString &name, const QString &serverAddress, const uint16_t &serverPort)
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

uint Server::getDevicesCount() const
{
    return _devices.size();
}

uint Server::getDeviceOnline() const
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
}

qint64 Server::writeData(QByteArray &data)
{
    return _socket.write(data);
}

void Server::deviceListShow(const QIcon &windowIcon)
{
    _deviceList = std::make_unique<DeviceList>(_devices);
    _deviceList->setWindowIcon(windowIcon);
    _deviceList->setWindowTitle(_serverAddress + ":" + QString::number(_serverPort));

    _deviceList->showMaximized();
    _deviceList->restructWidget();
}

void Server::createDevice(QByteArray &data)
{
    data = data.mid(2);
    uint8_t nameLength = data.at(0) >> 3;
    QString name = data.mid(1, nameLength);

    QByteArray buff;
    buff.append(0x05);
    buff.append(data.mid(1 + nameLength));

    _devices[name] = std::make_shared<Device>(*this, name, buff.at(1), this);
    IOTV_SH::response_WAY(*_devices[name], buff);

    const auto it = _alias.find(name);
    if(it != _alias.cend())
        _devices[name]->setViewName(_alias.at(name));

    if(_deviceList.get() != nullptr &&  _deviceList->isVisible()) //???
    {
        _deviceList->clear();
        _deviceList->restructWidget();
    }

    emit signalDeviceCreated();
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

void Server::slotConnected()
{
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

    _deviceList.reset();
    _devices.clear();

    notify();
}

void Server::slotReadData()
{
    QByteArray data = _socket.readAll();

    Log::write("Data recived form " + _socket.peerAddress().toString() + ":"
               + QString::number(_socket.peerPort())
               + " <- " + data.toHex(':'));

    for (auto &packetData : IOTV_SC::splitResponseData(data))
    {
        IOTV_SC::Response_Type dataType = IOTV_SC::checkResponsetData(packetData);

        if(dataType == IOTV_SC::Response_Type::RESPONSE_DEVICE_LIST)
            createDevice(packetData);
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
    }

    notify();
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

    Log::write(_name + ": " + strErr);
    //    notify();
}
