#include "client.h"

Client::Client(QObject *parent): QObject{parent},
    _stateConnection{false}
{
    _socket.setParent(this);
    _timerDevList.setParent(this);
    _connectWait.setParent(this);

    connect(&_socket, &QTcpSocket::connected, this, &Client::slotConnected, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::disconnected, this, &Client::slotDisconnected, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::readyRead, this, &Client::slotReciveData, Qt::QueuedConnection);

    connect(&_timerDevList, &QTimer::timeout, this, &Client::slotQueryDevList, Qt::QueuedConnection);
    connect(&_connectWait, &QTimer::timeout, this, &Client::slotConnectWait, Qt::QueuedConnection);
}

Client::~Client()
{

}

void Client::connectToHost(const QString &address, qint64 port)
{
    _socket.connectToHost(address, port);
    _connectWait.start(TIME_OUT);
}

void Client::disconnectFromHost()
{
    _socket.disconnectFromHost();
}

qint64 Client::writeData(const QByteArray &data)
{
    return _socket.write(data);
}

int Client::countDevices() const
{
    return _devices.size();
}

int Client::countDeviceOnline() const
{
    auto count = std::count_if(_devices.begin(), _devices.end(), [](const auto &pair)
    {
        return pair.second.isOnline();
    });

    return count;
}

QByteArray Client::readData(const QString &deviceName, uint8_t channelNumber) const
{
    if (_devices.count(deviceName) == 0)
        return {};

    return _devices.at(deviceName).getReadChannelData(channelNumber);
}

void Client::response_DEV_LIST(IOTV_SC::RESPONSE_PKG *pkg)
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Response_Type::RESPONSE_DEVICE_LIST))
        return;

    auto *responsePkg = static_cast<IOTV_SC::RESPONSE_DEV_LIST_PKG*>(pkg);

    for (const auto &dev : responsePkg->devs)
    {
        auto result = _devices.emplace(dev.name, dev);
        if (result.second)
        {
            result.first->second.setParent(this);
            connect(&result.first->second, &Device::signalQueryRead, this, &Client::slotQueryRead, Qt::QueuedConnection);
            connect(&result.first->second, &Device::signalQueryState, this, &Client::slotQueryState, Qt::QueuedConnection);
            connect(&result.first->second, &Device::signalQueryWrite, this, &Client::slotQueryWrite, Qt::QueuedConnection);
        }
        else
        {
            Device &oldDev = result.first->second;
            auto d = Device(dev);
            if (!(oldDev == d))
                oldDev.update(dev);

//            QString str = QString(Q_FUNC_INFO) + " " + dev.name + " can't create new device";
//            Log::write(str, Log::Write_Flag::FILE_STDOUT);
        }
    }
}

void Client::response_STATE(IOTV_SC::RESPONSE_PKG *pkg)
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Response_Type::RESPONSE_STATE))
        return;

    auto *responsePkg = static_cast<IOTV_SC::RESPONSE_STATE_PKG*>(pkg);

    if (!_devices.contains(responsePkg->name))
    {
//        QString str = QString(Q_FUNC_INFO) + " " + responsePkg->name + " not found in device list!";
//        Log::write(str, Log::Write_Flag::FILE_STDOUT);
        return;
    }

    int count = countDeviceOnline();
    _devices[responsePkg->name].setState(responsePkg->state);

    if (count != countDeviceOnline())
        emit onlineDeviceChanged();
}

void Client::response_READ(IOTV_SC::RESPONSE_PKG *pkg)
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Response_Type::RESPONSE_READ))
        return;

    auto *responsePkg = static_cast<IOTV_SC::RESPONSE_READ_PKG*>(pkg);

    if (!_devices.contains(responsePkg->name))
    {
//        QString str = QString(Q_FUNC_INFO) + " " + responsePkg->name + " not found in device list!";
//        Log::write(str, Log::Write_Flag::FILE_STDOUT);
        return;
    }

    _devices[responsePkg->name].setData(responsePkg->channelNumber, responsePkg->data);
}

void Client::response_WRITE(IOTV_SC::RESPONSE_PKG *pkg) const
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Response_Type::RESPONSE_WRITE))
        return;

    //Нет никакой реакции на ответ о записи
}

void Client::write(const QByteArray &data)
{
    if (data.isEmpty())
        return;

//    Log::write("Client transmit to server" + _socket.peerAddress().toString() + ':'
//               + QString::number(_socket.peerPort())
//               + " -> " + data.toHex(':'), Log::Write_Flag::FILE_STDOUT);
    _socket.write(data);
}

void Client::slotConnected()
{
    _connectWait.stop();

//    Log::write("Connected to " +
//               _socket.peerAddress().toString() +
//               ':' +
//               QString::number(_socket.peerPort()),
//               Log::Write_Flag::FILE_STDOUT);

    write(IOTV_SC::Client_TX::query_Device_List());
    _timerDevList.start(TIME_OUT);

    setStateConnection(true);
    emit signalConnected();
}

void Client::slotDisconnected()
{
//    Log::write("Disconnected from " +
//               _socket.peerAddress().toString() +
//               ':' +
//               QString::number(_socket.peerPort()),
//               Log::Write_Flag::FILE_STDOUT);

    _timerDevList.stop();

    _devices.clear();

    setStateConnection(false);
    emit countDeviceChanged();
    emit onlineDeviceChanged();
    emit signalDisconnected();
}

QList<QObject *> Client::devList()
{
    QList<QObject *> list;

    for(auto &[key, val] : _devices)
        list << &val;

    return list;
}

QObject *Client::deviceByName(QString name)
{
    return &_devices.find(name)->second;
}

bool Client::stateConnection() const
{
    return _stateConnection;
}

void Client::setStateConnection(bool newStateConnection)
{
    if (_stateConnection == newStateConnection)
        return;

    _stateConnection = newStateConnection;
    emit stateConnectionChanged();
}

void Client::slotReciveData()
{
    _recivedBuff += _socket.readAll();

//    Log::write("Data recive from " +
//               _socket.peerAddress().toString() +
//               ':' +
//               QString::number(_socket.peerPort()) +
//               " <- " +
//               _recivedBuff.toHex(':'),
//               Log::Write_Flag::FILE_STDOUT);

    IOTV_SC::RESPONSE_PKG *pkg;
    while ((pkg = IOTV_SC::Client_RX::accumPacket(_recivedBuff)) != nullptr)
    {
        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_INCOMPLETE)
        {
            delete pkg;
            break;
        }

        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_ERROR)
        {
            if (_recivedBuff.size() > 0)
            {
//                Log::write("WARRNING: received data from " +
//                           _socket.peerName() +
//                           _socket.peerAddress().toString() +
//                           ':' +
//                           QString::number(_socket.peerPort()) +
//                           "UNKNOW: " +
//                           _recivedBuff.toHex(':'),
//                           Log::Write_Flag::FILE_STDOUT);
                _recivedBuff.clear();
            }
            delete pkg;
            break;
        }

        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_DEVICE_LIST)
        {
            size_t count = _devices.size();

            response_DEV_LIST(pkg);

            if (count != _devices.size())
                emit countDeviceChanged();
        }
        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_STATE)
            response_STATE(pkg);
        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_READ)
            response_READ(pkg);
        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_WRITE)
            response_WRITE(pkg);
        else
        {
            //иных вариантов быть не должно!
//            Log::write(QString(Q_FUNC_INFO) +
//                       "Unknow pkg.type = " +
//                       QString::number(int(pkg->type)),
//                       Log::Write_Flag::FILE_STDERR);
            exit(-1);
        }
        delete pkg;
    }
}

void Client::slotQueryRead()
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    QByteArray data;
    for (uint8_t i = 0; i < dev->getReadChannelLength(); i++)
        data += IOTV_SC::Client_TX::query_READ(dev->getName(), i);

    write(data);
}

void Client::slotQueryState()
{
    Device *dev = qobject_cast<Device*>(sender());

    if (dev == nullptr)
        return;

    if (_socket.state() != QAbstractSocket::ConnectedState)
    {
        dev->setState(false);
        return;
    }

    QByteArray data;
    for (uint8_t i = 0; i < dev->getReadChannelLength(); i++)
        data += IOTV_SC::Client_TX::query_STATE(dev->getName());

    write(data);
}

void Client::slotQueryWrite(int channelNumber, QByteArray data)
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    write(IOTV_SC::Client_TX::query_WRITE(dev->getName(), channelNumber, data));
}

void Client::slotQueryDevList()
{
    write(IOTV_SC::Client_TX::query_Device_List());
}

void Client::slotError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
}

void Client::slotConnectWait()
{
    emit signalConnectWait();
    _connectWait.stop();
    disconnectFromHost();


}
