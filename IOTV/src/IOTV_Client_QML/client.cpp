#include "client.h"

Client::Client(QObject *parent): QObject{parent},
    TIME_OUT(5000),
    _stateConnection{false},
    _expectedDataSize(0)
{
    _socket.setParent(this);
    _socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    _timerDevList.setParent(this);
    _connectWait.setParent(this);
    _timerPing.setParent(this);

    connect(&_socket, &QTcpSocket::connected, this, &Client::slotConnected, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::disconnected, this, &Client::slotDisconnected, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::readyRead, this, &Client::slotReciveData, Qt::QueuedConnection);

    connect(&_socket, &QTcpSocket::stateChanged, this, &Client::slotStateChanged, Qt::QueuedConnection);

    connect(&_timerDevList, &QTimer::timeout, this, &Client::slotQueryDevList, Qt::QueuedConnection);
    connect(&_connectWait, &QTimer::timeout, this, &Client::slotConnectWait, Qt::QueuedConnection);
    connect(&_timerPing, &QTimer::timeout, this, &Client::disconnectFromHost, Qt::QueuedConnection);

    _timerDevList.start(TIME_OUT);
    _timerPing.start(TIME_OUT * 2);
}

Client::~Client()
{

}

void Client::connectToHost(const QString &address, qint64 port)
{
    disconnectFromHost();
    _socket.connectToHost(address, port);
    _connectWait.start(TIME_OUT);
}

void Client::disconnectFromHost()
{
    _socket.abort();
    //        _socket.disconnectFromHost();
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
    return std::count_if(_devices.begin(), _devices.end(), [](const auto &pair)
    {
        return pair.second.isOnline();
    });
}

QByteArray Client::readData(const QString &deviceName, uint8_t channelNumber) const
{
    if (_devices.count(deviceName) == 0)
        return {};

    return _devices.at(deviceName).getReadChannelData(channelNumber);
}

void Client::write(const QByteArray &data)
{
    if (!data.isEmpty())
        _socket.write(data);
}

void Client::slotConnected()
{
    _connectWait.stop();

    queryIdentification();

    setStateConnection(true);
    emit signalConnected();
}

void Client::slotDisconnected()
{
    _devices.clear();

    setStateConnection(false);
    emit countDeviceChanged();
    emit onlineDeviceChanged();
    emit signalDisconnected();
}

void Client::slotStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << socketState;
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

void Client::queryIdentification()
{
    char outData[BUFSIZ];
    auto size = queryIdentificationData(outData, BUFSIZ);

    write({outData, static_cast<int>(size)});
}

void Client::queryState(const QString &name)
{
    char outData[BUFSIZ];
    auto size = queryStateData(outData, BUFSIZ, name.toStdString().c_str());

    write({outData, static_cast<int>(size)});
}

void Client::queryRead(const QString &name, uint8_t channelNumber)
{
    char outData[BUFSIZ];
    auto size = queryReadData(outData, BUFSIZ, name.toStdString().c_str(), channelNumber);

    write({outData, static_cast<int>(size)});
}

void Client::queryWrite(const QString &name, uint8_t channelNumber, const QByteArray &data)
{
    char outData[BUFSIZ];
    auto size = queryWriteData(outData, BUFSIZ, name.toStdString().c_str(), channelNumber,
                               data.data(), data.size());

    write({outData, static_cast<int>(size)});
}

void Client::queryPingPoing()
{
    char outData[BUFSIZ];
    auto size = queryPingData(outData, BUFSIZ);

    write({outData, static_cast<int>(size)});
}

void Client::responceIdentification(const Header *header)
{
    Q_ASSERT(header != NULL);

    struct IOTV_Server_embedded *iot = createIotFromHeaderIdentification(header);
    QString name = QByteArray{header->identification->name, header->identification->nameSize};

    if (!_devices.contains(name))
    {
        auto result = _devices.emplace(name, iot);
        if (result.second)
        {
            result.first->second.setParent(this);
            connect(&result.first->second, &Device::signalQueryRead, this, &Client::slotQueryRead, Qt::QueuedConnection);
            connect(&result.first->second, &Device::signalQueryState, this, &Client::slotQueryState, Qt::QueuedConnection);
            connect(&result.first->second, &Device::signalQueryWrite, this, &Client::slotQueryWrite, Qt::QueuedConnection);
        }
    }
    else
    {
        /*Device &oldDev = */_devices[name].update(iot);
        //        auto d = Device(iot);
        //        if (oldDev != d)
        //            oldDev.update(iot);
    }

    clearIOTV_Server(iot);
}

void Client::responceState(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->state != NULL);

    QString name = QByteArray(header->state->name, header->state->nameSize);

    if (!_devices.contains(name))
        return;

    if (_devices[name].state() != header->state->state)
    {
        _devices[name].setState(header->state->state);
//        emit _devices[name].stad
    }
}

void Client::responceRead(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->readWrite != NULL);

    QString name = QByteArray(header->readWrite->name, header->readWrite->nameSize);

    if (!_devices.contains(name))
        return;

    QByteArray data;
    //    if (isLittleEndian() && byteOrderReversebleData(static_cast<uint8_t>(_devices[name].getReadChannelType(header->readWrite->channelNumber))))
    //        dataReverse((void *)header->readWrite->data, header->readWrite->dataSize);
    data.append(QByteArray{header->readWrite->data, static_cast<int>(header->readWrite->dataSize)});

    _devices[name].setData(header->readWrite->channelNumber, data);
}

void Client::responceWrite(const struct Header *header) const
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->readWrite != NULL);
}

void Client::responcePingPoing(const struct Header *header)
{
    Q_ASSERT(header != NULL);
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
    _timerPing.start();
    _recivedBuff += _socket.readAll();

    bool error = false;
    uint64_t cutDataSize = 0;

    while (_recivedBuff.size() > 0)
    {
        struct Header* header = createPkgs(reinterpret_cast<uint8_t*>(_recivedBuff.data()), _recivedBuff.size(), &error, &_expectedDataSize, &cutDataSize);

        if (error == true)
        {
            _recivedBuff.clear();
            _expectedDataSize = 0;
            cutDataSize = 0;
            clearHeader(header);
            break;
        }

        // Пакет не ещё полный
        if (_expectedDataSize > 0)
        {
            clearHeader(header);
            break;
        }

        if (header->type == Header::HEADER_TYPE_RESPONSE)
        {
            if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
            {
                size_t count = _devices.size();
                responceIdentification(header);
                if (count != _devices.size())
                    emit countDeviceChanged();
            }
            else if(header->assignment == Header::HEADER_ASSIGNMENT_READ)
                responceRead(header);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
                responceWrite(header);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_PING_PONG)
                responcePingPoing(header);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_STATE)
                responceState(header);

            //            printHeader(header);
        }
        else if(header->type == Header::HEADER_TYPE_REQUEST)
        {
            // На данный момент от сервера не должно приходить ответов
            Log::write("Ответ от сервера не предусмотрен!");
        }

        _recivedBuff = _recivedBuff.mid(cutDataSize);
        clearHeader(header);
    }



    //    Log::write("Data recive from " +
    //               _socket.peerAddress().toString() +
    //               ':' +
    //               QString::number(_socket.peerPort()) +
    //               " <- " +
    //               _recivedBuff.toHex(':'),
    //               Log::Write_Flag::FILE_STDOUT);

    //    IOTV_SC::RESPONSE_PKG *pkg;
    //    while ((pkg = IOTV_SC::Client_RX::accumPacket(_recivedBuff)) != nullptr)
    //    {
    //        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_INCOMPLETE)
    //        {
    //            delete pkg;
    //            break;
    //        }

    //        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_ERROR)
    //        {
    //            if (_recivedBuff.size() > 0)
    //            {
    //                //                Log::write("WARRNING: received data from " +
    //                //                           _socket.peerName() +
    //                //                           _socket.peerAddress().toString() +
    //                //                           ':' +
    //                //                           QString::number(_socket.peerPort()) +
    //                //                           "UNKNOW: " +
    //                //                           _recivedBuff.toHex(':'),
    //                //                           Log::Write_Flag::FILE_STDOUT);
    //                _recivedBuff.clear();
    //            }
    //            delete pkg;
    //            break;
    //        }

    //        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_DEVICE_LIST)
    //        {
    //            size_t count = _devices.size();

    //            response_DEV_LIST(pkg);

    //            if (count != _devices.size())
    //                emit countDeviceChanged();
    //        }
    //        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_STATE)
    //            response_STATE(pkg);
    //        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_READ)
    //            response_READ(pkg);
    //        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_WRITE)
    //            response_WRITE(pkg);
    //        else
    //        {
    //            //иных вариантов быть не должно!
    //            //            Log::write(QString(Q_FUNC_INFO) +
    //            //                       "Unknow pkg.type = " +
    //            //                       QString::number(int(pkg->type)),
    //            //                       Log::Write_Flag::FILE_STDERR);
    //            exit(-1);
    //        }
    //        delete pkg;
    //    }
}

void Client::slotQueryRead()
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    for (uint8_t i = 0; i < dev->getReadChannelLength(); i++)
        queryRead(dev->getName(), i);

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

    queryState(dev->getName());
}

void Client::slotQueryWrite(int channelNumber, QByteArray data)
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    queryWrite(dev->getName(), channelNumber, data);
}

void Client::slotQueryDevList()
{
    queryIdentification();
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
