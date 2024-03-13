#include "client.h"

#include "iotv_event_manager.h"
#include "event_action_parser.h"
#include "log.h"
#include "IOTV_SH.h"
#include "creatorpkgs.h"
#include "identification.h"
#include "state.h"
#include "read_write.h"
#include "tech.h"

#include <iostream>
#include <fstream>
#include <QTemporaryFile>

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

QList<QList<QVariantMap> > Client::replaceRealNameToAlias(const QList<QList<QVariantMap> > &evActList) const
{
    QList<QList<QVariantMap> > result = evActList;

    for (auto &list : result)
    {
        if (list.size() < 2)
            continue;

        // У событий
        list[0][Json_Event_Action::HOST_NAME] = findAliasName(list.at(0)[Json_Event_Action::HOST_NAME].toString());

        // У действий
        if (list.at(1)[Json_Event_Action::TYPE].toString() == Json_Event_Action::TYPE_DATA_TX)
        {
            list[1][Json_Event_Action::HOST_NAME] = findAliasName(list.at(1)[Json_Event_Action::HOST_NAME].toString());
        }
        else if (list.at(1)[Json_Event_Action::TYPE].toString() == Json_Event_Action::TYPE_DATA_TX_REF)
        {
            list[1][Json_Event_Action::HOST_SRC] = findAliasName(list.at(1)[Json_Event_Action::HOST_SRC].toString());
            list[1][Json_Event_Action::HOST_DST] = findAliasName(list.at(1)[Json_Event_Action::HOST_DST].toString());
        }
    }

    return result;
}

QList<QList<QVariantMap> > Client::replaceAliasToRealName(const QList<QList<QVariantMap> > &evActList) const
{
    QList<QList<QVariantMap> > result = evActList;

    for (auto &list : result)
    {
        if (list.size() < 2)
            continue;

        // У событий
        list[0][Json_Event_Action::HOST_NAME] = findRealName(list.at(0)[Json_Event_Action::HOST_NAME].toString());

        // У действий
        if (list.at(1)[Json_Event_Action::TYPE].toString() == Json_Event_Action::TYPE_DATA_TX)
        {
            list[1][Json_Event_Action::HOST_NAME] = findRealName(list.at(1)[Json_Event_Action::HOST_NAME].toString());
        }
        else if (list.at(1)[Json_Event_Action::TYPE].toString() == Json_Event_Action::TYPE_DATA_TX_REF)
        {
            list[1][Json_Event_Action::HOST_SRC] = findRealName(list.at(1)[Json_Event_Action::HOST_SRC].toString());
            list[1][Json_Event_Action::HOST_DST] = findRealName(list.at(1)[Json_Event_Action::HOST_DST].toString());
        }
    }

    return result;
}

QString Client::findRealName(const QString &alias) const
{
    for (auto &pair : _devices)
    {
        if (pair.second.aliasName() == alias)
            return pair.second.getName();
    }

    return alias;
}

QString Client::findAliasName(const QString &realName) const
{
    if (_devices.contains(realName))
        return _devices.at(realName).aliasName();

    return realName;
}

void Client::removeEventAction(QList<QList<QVariantMap> > &list, const QString &name)
{
    for (auto i = 0; i < list.size(); ++i)
    {
        if (list[i].size() < 2)
            continue;

        if (list[i][0][Json_Event_Action::EVENT_ACTION_NAME] == name)
            list.removeAt(i);
    }
}

std::forward_list<const Base_Host *> Client::host_list() const
{
    std::forward_list<const Base_Host *> hosts;
    for(const auto &pair : _devices)
        hosts.push_front(&pair.second);

    return hosts;
}

std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *> > > Client::convert(const QList<QList<QVariantMap> > &list) const
{
    std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>> result;
    auto hosts = host_list();

    for (auto &el : list)
    {
        QString el_name = el.at(0)[Json_Event_Action::EVENT_ACTION_NAME].toString();
        IOTV_Event *el_event = IOTV_Event_Manager::createEvent(el.at(0), hosts);
        IOTV_Action *el_action = IOTV_Event_Manager::createAction(el.at(1), hosts);

        result.emplace_front(el_name, std::make_pair(el_event, el_action));
    }

    return result;
}

void Client::clearList(std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *> > > &list) const
{
    for (auto &el : list)
    {
        auto event = el.second.first;
        auto action = el.second.second;

        if (event != nullptr)
            delete event;

        if (action != nullptr)
            delete action;
    }
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

void Client::slotOpenReadStream(int channel)
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    char outData[BUFSIZ];
    auto size = queryReadData(outData, BUFSIZ, dev->getName().toStdString().c_str(), channel, ReadWrite_FLAGS_OPEN_STREAM);

    write({outData, static_cast<int>(size)});
}

void Client::slotCloseReadStream(int channel)
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    char outData[BUFSIZ];
    auto size = queryReadData(outData, BUFSIZ, dev->getName().toStdString().c_str(), channel, ReadWrite_FLAGS_CLOSE_STREAM);

    write({outData, static_cast<int>(size)});
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

QList<QList<QVariantMap>> Client::evAcList() const
{
    return _evAcList;
}

void Client::saveEventAction(QVariantMap event, QVariantMap action,  QString oldName)
{
    auto host = host_list();

    {
        QList<QVariantMap> list;
        list << event << action;
        auto l = replaceAliasToRealName({list});
        event = l[0][0];
        action = l[0][1];
    }

    QString name = event[Json_Event_Action::EVENT_ACTION_NAME].toString();
    IOTV_Event *_event = IOTV_Event_Manager::createEvent(event, host);
    IOTV_Action *_action = IOTV_Event_Manager::createAction(action, host);

    auto evAcListWithRealName = replaceAliasToRealName(_evAcList);

    removeEventAction(evAcListWithRealName, name);
    removeEventAction(evAcListWithRealName, oldName);

    auto list = convert(evAcListWithRealName);
    list.emplace_front(name, std::make_pair(_event, _action));

    QByteArray data = Event_Action_Parser::toData(list);
    queryTech(Tech_TYPE_EV_AC, data.data(), data.size());

    clearList(list);
}

void Client::removeEventAction(QString name)
{
    auto evAcListWithRealName = replaceAliasToRealName(_evAcList);
    removeEventAction(evAcListWithRealName, name);

    auto list = convert(evAcListWithRealName);

    QByteArray data = Event_Action_Parser::toData(list);
    queryTech(Tech_TYPE_EV_AC, data.data(), data.size());

    clearList(list);
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
            connect(&device, &Device::signalOpenReadStream, this, &Client::slotOpenReadStream);
            connect(&device, &Device::signalCloseReadStream, this, &Client::slotCloseReadStream);
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
    if (pkg == nullptr)
        return;

//    qDebug() << "PKG " << header->fragment << "(" << pkg->dataSize <<  ") /" << header->fragments ;

    auto channel = pkg->channelNumber;
    QString name(QByteArray{pkg->name, pkg->nameSize});

    if (!_devices.contains(name))
        return;

    if (pkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
        responceReadStream(header);
    else
        _devices[name].setData(channel, {pkg->data, static_cast<int>(pkg->dataSize)});
}

void Client::responceReadStream(const Header *header)
{
    const struct Read_Write *pkg = static_cast<const struct Read_Write *>(header->pkg);
    if (pkg == nullptr)
        return;

    auto channel = pkg->channelNumber;
    QString name(QByteArray{pkg->name, pkg->nameSize});

    if (!_devices.contains(name))
        return;

    if (header->fragment == 1)
        _devices[name].clearData(channel);

    _devices[name].addData(channel, {pkg->data, static_cast<int>(pkg->dataSize)});

    if (header->fragment == header->fragments)
    {
        QByteArray data = _devices[name].getReadChannelData(channel);
        qDebug() << "PKG total: " << data.size();
        emit _devices[name].signalDataPkgComplete(channel, std::move(data));
    }
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

        _evAcList = Event_Action_Parser::parseJsonToVariantMap(data, hosts);
        _evAcList = replaceRealNameToAlias(_evAcList);

        emit signalEventAction();
    }
}

void Client::slotReciveData()
{
    while (_socket.bytesAvailable())
        _recivedBuff += _socket.readAll();

//    _recivedBuff = _socket.readAll();

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
