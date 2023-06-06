#include "client.h"

Client::Client(QObject *parent): QObject{parent},
    _expectedDataSize(0), _counterPing(0)
{
    _socket.setParent(this);
    _socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    _timerPing.setParent(this);
    _timerPing.setInterval(Base_Host::TIMER_PING_INTERVAL);

    connect(&_socket, &QTcpSocket::connected, this, &Client::slotConnected);
    connect(&_socket, &QTcpSocket::disconnected, this, &Client::slotDisconnected);
    connect(&_socket, &QTcpSocket::readyRead, this, &Client::slotReciveData);
    connect(&_socket, &QTcpSocket::stateChanged, this, &Client::slotStateChanged);

    connect(&_timerPing, &QTimer::timeout, this, &Client::queryPing);
}

Client::~Client()
{

}

void Client::connectToHost(const QString &address, qint64 port)
{
    disconnectFromHost();
    _socket.connectToHost(address, port);
    _timerPing.start();
}

void Client::disconnectFromHost()
{
    _socket.abort();
    _timerPing.stop();
    _counterPing = 0;
}

int Client::countDevices() const
{
    return _devices.size();
}

int Client::countDeviceOnline() const
{
    return std::count_if (_devices.begin(), _devices.end(), [](const auto &pair)
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

qint64 Client::write(const QByteArray &data)
{
    if (!stateConnection())
        return -1;

    return _socket.write(data);
}

void Client::slotConnected()
{
    emit signalConnected();
    emit stateConnectionChanged();

    slotQueryIdentification();    
}

void Client::slotDisconnected()
{
    _devices.clear();
    _expectedDataSize = 0;
    _recivedBuff.clear();

    emit countDeviceChanged();
    emit onlineDeviceChanged();
    emit stateConnectionChanged();
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

QObject *Client::deviceByName(const QString &name)
{
    auto it = _devices.find(name);
    if (it == _devices.end())
        return nullptr;

    return &it->second;
}

void Client::queryEventAction()
{
    queryTech(Tech_TYPE_EV_AC, nullptr, 0);
}

bool Client::stateConnection() const
{
    return _socket.state() == QAbstractSocket::ConnectedState;
}

QList<QList<QObject*>> Client::evAcList() const
{
    return _evAcList;
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
    auto size = queryReadData(outData, BUFSIZ, name.toStdString().c_str(), channelNumber, ReadWrite_FLAGS_NONE);

    write({outData, static_cast<int>(size)});
}

void Client::queryWrite(const QString &name, uint8_t channelNumber, const QByteArray &data)
{
    char outData[BUFSIZ];
    auto size = queryWriteData(outData, BUFSIZ, name.toStdString().c_str(), channelNumber,
                               data.data(), data.size());

    write({outData, static_cast<int>(size)});
}

void Client::queryPing()
{
    _counterPing++;

    char outData[BUFSIZ];
    auto size = queryPingData(outData, BUFSIZ);

    write({outData, static_cast<int>(size)});

    if (_counterPing > COUNTER_PING_COUNT)
    {
        Log::write(" WARRNING: ping timeout",
                   Log::Write_Flag::STDOUT, "");
        disconnectFromHost();
    }
}

void Client::queryTech(Tech_TYPE type, char *data, uint64_t dataSize)
{
    char outData[BUFSIZ];
    auto size = ::queryTech(outData, BUFSIZ, data, dataSize, type);

    write({outData, static_cast<int>(size)});
}

void Client::responceIdentification(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct Identification *pkg = static_cast<const struct Identification *>(header->pkg);

    struct IOTV_Server_embedded *iot = createIotFromHeaderIdentification(header);
    QString name(QByteArray{pkg->name, pkg->nameSize});

    if (!_devices.contains(name))
    {
        auto result = _devices.emplace(name, iot);
        if (result.second)
        {
            Device &device = result.first->second;
            device.setParent(this);
            connect(&device, &Device::signalQueryIdentification, this, &Client::slotQueryIdentification);
            connect(&device, &Device::signalQueryRead, this, &Client::slotQueryRead);
            connect(&device, &Device::signalQueryState, this, &Client::slotQueryState);
            connect(&device, &Device::signalQueryWrite, this, &Client::slotQueryWrite);
        }
    }
    else
        _devices[name].update(iot);

    clearIOTV_Server(iot);
}

void Client::responceState(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct State *pkg = static_cast<const struct State *>(header->pkg);

    QString name(QByteArray{pkg->name, pkg->nameSize});

    if (!_devices.contains(name))
        return;

    if (_devices[name].state() != pkg->state)
    {
        _devices[name].setState(pkg->state);
        emit onlineDeviceChanged();
    }
}

void Client::responceRead(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct Read_Write *pkg = static_cast<const struct Read_Write *>(header->pkg);

    QString name(QByteArray{pkg->name, pkg->nameSize});

    if (!_devices.contains(name))
        return;

    _devices[name].setData(pkg->channelNumber,
                           {pkg->data, static_cast<int>(pkg->dataSize)});
}

void Client::responceWrite(const struct Header *header) const
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    // Нет реакции на ответ о записи
}

void Client::responcePingPoing(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    _counterPing = 0;
}

void Client::responceTech(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct Tech *pkg = static_cast<const struct Tech*>(header->pkg);

    if (pkg->type == Tech_TYPE_EV_AC)
    {
        QByteArray data(reinterpret_cast<const char *>(pkg->data), pkg->dataSize);

        std::forward_list<const Base_Host *> hosts;
        for(const auto &pair : _devices)
            hosts.push_front(&pair.second);

        auto list = Event_Action_Parser::parseJson(data, hosts);

        _evAcList.clear();

        for (const auto &el : list)
        {
            QList<QObject *> l;
            l << el.second.first << el.second.second;
            l[0]->setObjectName(el.first);
            _evAcList << l;
        }

        emit signalEventAction();
    }
}

void Client::slotReciveData()
{
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

        if (header->type == HEADER_TYPE_RESPONSE)
        {
            if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
            {
                size_t count = _devices.size();
                responceIdentification(header);
                if (count != _devices.size())
                    emit countDeviceChanged();
            }
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
                responceRead(header);
            else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
                responceWrite(header);
            else if (header->assignment == HEADER_ASSIGNMENT_PING_PONG)
                responcePingPoing(header);
            else if (header->assignment == HEADER_ASSIGNMENT_STATE)
                responceState(header);
            else if (header->assignment == HEADER_ASSIGNMENT_TECH)
                responceTech(header);
        }
        else if (header->type == HEADER_TYPE_REQUEST)
        {
            // На данный момент от сервера не должно приходить запросов
            Log::write("Запрос от сервера не предусмотрен!", Log::Write_Flag::STDOUT, "");
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

void Client::slotQueryWrite(int channelNumber, const QByteArray &data)
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    queryWrite(dev->getName(), channelNumber, data);
}

void Client::slotQueryIdentification()
{
//    Device *dev = qobject_cast<Device*>(sender());
    queryIdentification();
}

void Client::slotError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
}
