#include "client.h"

Client::Client(QObject *parent): QObject{parent},
    _stateConnection{false},
    _expectedDataSize(0)
{
    _socket.setParent(this);
    _socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
//    _timerServerUnavailable.setParent(this);

    connect(&_socket, &QTcpSocket::connected, this, &Client::slotConnected, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::disconnected, this, &Client::slotDisconnected, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::readyRead, this, &Client::slotReciveData, Qt::QueuedConnection);

    connect(&_socket, &QTcpSocket::stateChanged, this, &Client::slotStateChanged, Qt::QueuedConnection);

//    connect(&_timerServerUnavailable, &QTimer::timeout, this, &Client::disconnectFromHost, Qt::QueuedConnection);

//    _timerServerUnavailable.start(TIME_OUT);
}

Client::~Client()
{

}

void Client::connectToHost(const QString &address, qint64 port)
{
    disconnectFromHost();
    _socket.connectToHost(address, port);
}

void Client::disconnectFromHost()
{
    _socket.abort();
}

//qint64 Client::writeData(const QByteArray &data)
//{
//    return _socket.write(data);
//}

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
    if (!stateConnection())
        return;



    _socket.write(data);
}

void Client::slotConnected()
{
    setStateConnection(true);
    emit signalConnected();

    slotQueryIdentification();
}

void Client::slotDisconnected()
{
    _devices.clear();

    setStateConnection(false);
    //!!! Не нужно сообщать про количество устройств, так как они обнулятся
    emit countDeviceChanged();
    emit onlineDeviceChanged();
}

void Client::slotStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << socketState;

    if (socketState == QAbstractSocket::UnconnectedState)
        emit signalDisconnected();
    else if (socketState == QAbstractSocket::ConnectingState)
        emit signalConnecting();
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
    auto it = _devices.find(name);
    if (it == _devices.end())
        return nullptr;

    return &it->second;
}

bool Client::stateConnection() const
{
    return _socket.state() == QAbstractSocket::ConnectedState;
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
    Q_ASSERT(header->identification != NULL);

    struct IOTV_Server_embedded *iot = createIotFromHeaderIdentification(header);
    QString name(QByteArray{header->identification->name, header->identification->nameSize});

    if (!_devices.contains(name))
    {
        auto result = _devices.emplace(name, iot);
        if (result.second)
        {
            Device &device = result.first->second;
            device.setParent(this);
            connect(&device, &Device::signalQueryIdentification, this, &Client::slotQueryIdentification, Qt::QueuedConnection);
            connect(&device, &Device::signalQueryRead, this, &Client::slotQueryRead, Qt::QueuedConnection);
            connect(&device, &Device::signalQueryState, this, &Client::slotQueryState, Qt::QueuedConnection);
            connect(&device, &Device::signalQueryWrite, this, &Client::slotQueryWrite, Qt::QueuedConnection);
        }
    }
    else
        _devices[name].update(iot);

    clearIOTV_Server(iot);
}

void Client::responceState(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->state != NULL);

    QString name(QByteArray{header->state->name, header->state->nameSize});

    if (!_devices.contains(name))
        return;

    if (_devices[name].state() != header->state->state)
    {
        _devices[name].setState(header->state->state);
        emit onlineDeviceChanged();
    }
}

void Client::responceRead(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->readWrite != NULL);

    QString name(QByteArray{header->readWrite->name, header->readWrite->nameSize});

    if (!_devices.contains(name))
        return;

    _devices[name].setData(header->readWrite->channelNumber,
                           {header->readWrite->data, static_cast<int>(header->readWrite->dataSize)});
}

void Client::responceWrite(const struct Header *header) const
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->readWrite != NULL);

    // Нет реакции на ответ о записи
}

void Client::responcePingPoing(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    // Нет реакции на ответ ping
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
//    _timerServerUnavailable.start();
    _recivedBuff += _socket.readAll();

    bool error = false;
    uint64_t cutDataSize = 0;

    while (_recivedBuff.size() > 0)
    {
        struct Header* header = createPkgs(reinterpret_cast<uint8_t*>(_recivedBuff.data()), _recivedBuff.size(),
                                           &error, &_expectedDataSize, &cutDataSize);

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
            // На данный момент от сервера не должно приходить запросов
            Log::write("Запрос от сервера не предусмотрен!");
        }

        _recivedBuff = _recivedBuff.mid(cutDataSize);
        clearHeader(header);
    }
}

void Client::slotQueryRead()
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    for (uint8_t i = 0; i < dev->getReadChannelLength(); ++i)
        queryRead(dev->getName(), i);

}

void Client::slotQueryState()
{
    Device *dev = qobject_cast<Device*>(sender());

    if (dev == nullptr)
        return;

    queryState(dev->getName());
}

void Client::slotQueryWrite(int channelNumber, QByteArray data)
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    queryWrite(dev->getName(), channelNumber, data);
}

void Client::slotQueryIdentification()
{
    queryIdentification();
}

void Client::slotError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
}
