#include "client.h"

#include "actions/iotv_action_data_tx.h"
#include "actions/iotv_action_data_tx_ref.h"
#include "events/iotv_event_alarm.h"
#include "events/iotv_event_connect.h"
#include "events/iotv_event_data.h"
#include "events/iotv_event_disconnect.h"
#include "events/iotv_event_state.h"
#include "events/iotv_event_timer.h"
#include "iotv_event_manager.h"
#include "event_action_parser.h"
#include "log.h"
#include "IOTV_SH.h"
#include "creatorpkgs.h"
#include "identification.h"
#include "state.h"
#include "read_write.h"
#include "tech.h"

#include <QtConcurrent>
#include<QTime>

#include <QApplication>

Client::Client(QObject *parent): QObject{parent},
    _counterPing(0)
{
    _socket.setParent(this);
    _socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    _timerPing.setParent(this);
    _timerPing.setInterval(Base_Host::TIMER_PING_INTERVAL);

    connect(&_socket, &QTcpSocket::connected, this, &Client::slotConnected, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::disconnected, this, &Client::slotDisconnected, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::readyRead, this, &Client::slotReciveData, Qt::QueuedConnection);
    connect(&_socket, &QTcpSocket::stateChanged, this, &Client::slotStateChanged, Qt::QueuedConnection);

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

QList<QObject *> Client::devList()
{
    QList<QObject *> result;

    for(auto &[key, val] : _devices)
        result << &val;

    return result;
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
    queryTech(Tech_TYPE_EV_AC, nullptr, 0);
    //    queryEventAction();
}

void Client::slotDisconnected()
{
    _devices.clear();
    _recivedBuff.clear();

    emit countDeviceChanged();
    emit onlineDeviceChanged();
    emit stateConnectionChanged();
}

void Client::slotStateChanged(QAbstractSocket::SocketState socketState)
{
//    qDebug() << socketState;

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

QList<QString> Client::allHostAliasName() const
{
    // имена всех исвестных хостов из IOTV_Event_Action.json
    std::set<QString> set = _eventManager->allHostsName();

    // удалить реальные имена существующих устройств
    std::erase_if(set, [&](auto &el){
        return _devices.contains(el);
    });

    // добавить алиасы существующих устройств
    for(const auto &[key, val] : _devices)
        set.insert(val.aliasName());

    // Пустое имя
    set.insert("");

    return {set.begin(), set.end()};
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

void Client::saveEventAction()
{
    QByteArray data = Event_Action_Parser::toData(_eventManager->events(), _eventManager->actions(), _eventManager->eventGroups(), _eventManager->actionGroups());
    queryTech(Tech_TYPE_EV_AC, data.data(), data.size());
    queryTech(Tech_TYPE_EV_AC, nullptr, 0);
}

void Client::saveEvent(IOTV_Event *event)
{
    if (event == nullptr)
        return;

    QString eventName(event->name());
    QString eventGroup(event->group());

    auto findEvent = _eventManager->findEvent(eventGroup, eventName);
    if (findEvent != nullptr)
        _eventManager->deleteEvent(eventGroup, eventName);

    // в Event.qml передаётся копия event, которая удаляется при уничтожении Event.qml
    // по этому делается копия
    IOTV_Event *copyEv = IOTV_Event_Manager::copyEvent(event);
    std::shared_ptr<IOTV_Event> ptr(copyEv);
    _eventManager->addEvent(ptr);

    saveEventAction();
}

void Client::removeEvent(IOTV_Event *event)
{
    if (event == nullptr)
        return;

    QString eventName = event->name();
    QString eventGroup = event->group();

    _eventManager->deleteEvent(eventGroup, eventName);
    saveEventAction();
}

void Client::removeEvent(const QString &groupName, const QString &eventName)
{
    _eventManager->deleteEvent(groupName, eventName);
    saveEventAction();
}

void Client::renameEventGroup(const QString &oldGroupName, const QString &newGroupName)
{
    _eventManager->renameEventGroup(oldGroupName, newGroupName);
    saveEventAction();
}

void Client::removeEventGroup(const QString &groupName)
{
    _eventManager->removeEventGroup(groupName);
    saveEventAction();
}

bool Client::isEmptyEventGroup(const QString &groupName)
{
    return _eventManager->eventsInGroup(groupName).size() == 0;
}

bool Client::isExistsEventGroup(const QString &groupName)
{
    return _eventManager->eventGroups().contains(groupName);
}

bool Client::isExistsEventNameInGroup(const QString &groupName, const QString &eventName)
{
    return _eventManager->findEvent(groupName, eventName) != nullptr;
}

void Client::saveEventGroup(const QString &groupName)
{
    _eventManager->addEventGroup(groupName);
    saveEventAction();
}

QList<QString> Client::eventsGroupList() const
{
    if (_eventManager.get() == nullptr)
        return {};

    const auto &event_groups = _eventManager->eventGroups();
    return {event_groups.begin(), event_groups.end()};
}

QList<QString> Client::actionsGroupList() const
{
    if (_eventManager.get() == nullptr)
        return {};

    const auto &action_groups = _eventManager->actionGroups();
    return {action_groups.begin(), action_groups.end()};
}

QList<QString> Client::eventsListInGroup(const QString &groupName) const
{
    if (_eventManager.get() == nullptr)
        return {};

    auto list = _eventManager->eventsInGroup(groupName);

    QList<QString> result;
    for (const auto &el : list)
        result.push_back(el->name());

    return result;
}

void Client::saveAction(IOTV_Action *action)
{
    if (action == nullptr)
        return;

    QString actionName(action->name());
    QString actionGroup(action->group());

    auto findEvent = _eventManager->findAction(actionGroup, actionName);
    if (findEvent != nullptr)
        _eventManager->deleteAction(actionGroup, actionName);

    // в Action.qml передаётся копия action, которая удаляется при уничтожении Action.qml
    // по этому делается копия
    IOTV_Action *copyAc = IOTV_Event_Manager::copyAction(action);
    std::shared_ptr<IOTV_Action> ptr(copyAc);
    _eventManager->addAction(ptr);

    saveEventAction();
}

void Client::saveActionGroup(const QString &groupName)
{
    _eventManager->addActionGroup(groupName);
    saveEventAction();
}

void Client::removeAction(const QString &groupName, const QString &eventName)
{
    _eventManager->deleteAction(groupName, eventName);
    saveEventAction();
}

void Client::removeActionGroup(const QString &groupName)
{
    _eventManager->removeActionGroup(groupName);
    saveEventAction();
}

void Client::renameActionGroup(const QString &oldGroupName, const QString &newGroupName)
{
    _eventManager->renameActionGroup(oldGroupName, newGroupName);
    saveEventAction();
}

bool Client::isEmptyActionGroup(const QString &groupName)
{
    return _eventManager->actionsInGroup(groupName).size() == 0;
}

bool Client::isExistsActionGroup(const QString &groupName)
{
    return _eventManager->actionGroups().contains(groupName);
}

bool Client::isExistsActionNameInGroup(const QString &groupName, const QString &eventName)
{
    return _eventManager->findAction(groupName, eventName) != nullptr;
}

QList<QString> Client::actionsListInGroup(const QString &groupName) const
{
    if (_eventManager.get() == nullptr)
        return {};

    const auto &list = _eventManager->actions();

    QList<QString> result;
    for (const auto &el : list)
    {
        if (el->group() == groupName)
            result.push_back(el->name());
    }

    return result;
}

IOTV_Event *Client::copyEventByNameAndGroup(const QString &eventName, const QString &groupName) const
{
    auto event = _eventManager->findEvent(groupName, eventName);

    if (event == nullptr)
        return nullptr;

    return IOTV_Event_Manager::copyEvent(event.get());
}

IOTV_Event *Client::createEmptyEvent(const QString &eventType, const QString &eventName, const QString &groupName) const
{
    IOTV_Event *result = nullptr;

    if (eventType == "NONE")
        result = new IOTV_Event(IOTV_Event::EVENT_TYPE::NONE, nullptr);
    else if (eventType == "CONNECTING") // NONE не надо возвращать nullptr!
        result = new IOTV_Event_Connect(nullptr);
    else if (eventType == "DISCONNECTING")
        result = new IOTV_Event_Disconnect(nullptr);
    else if (eventType == "STATE")
        result = new IOTV_Event_State(IOTV_Event_State::STATE_TYPE::NONE, nullptr);
    else if (eventType == "DATA")
        result = new IOTV_Event_Data(IOTV_Event_Data::DATA_DIRECTION::NONE, Json_Event_Action::COMPARE_ALWAYS_FALSE, nullptr, 0, "0");
    else if (eventType == "ALARM")
    {
        std::array<bool, 7> days;
        std::memset(days.data(), false, days.size());

        result = new IOTV_Event_Alarm({0, 0}, days);
    }
    else if (eventType == "TIMER")
        result = new IOTV_Event_Timer(0);

    if (result != nullptr)
    {
        result->setName(eventName);
        result->setGroup(groupName);
    }

    return result;
}

IOTV_Action *Client::copyActionByNameAndGroup(const QString &actionName, const QString &groupName) const
{
    auto action = _eventManager->findAction(groupName, actionName);

    if (action == nullptr)
        return nullptr;

    return IOTV_Event_Manager::copyAction(action.get());
}

IOTV_Action *Client::createEmptyAction(const QString &actionType, const QString &actionName, const QString &groupName) const
{
    IOTV_Action *result = nullptr;

    if (actionType == "NONE")
        result = new IOTV_Action(IOTV_Action::ACTION_TYPE::NONE, nullptr);
    else if (actionType == "DATA_TX")
        result = new IOTV_Action_Data_TX(nullptr, 0, "");
    else if (actionType == "DATA_TX_REF")
        result = new IOTV_Action_Data_TX_Ref(nullptr, 0, nullptr, 0);

    if (result != nullptr)
    {
        result->setName(actionName);
        result->setGroup(groupName);
    }

    return result;
}

void Client::deleteObject(QObject *obj) const
{
    obj->deleteLater();
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

void Client::queryLogDataHost(const QString &name, uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, log_data_flag_t flags)
{
    char outData[BUFSIZ];
    auto size = queryLogData(outData, BUFSIZ, name.toStdString().c_str(), startInterval, endInterval, interval, channelNumber, static_cast<uint8_t>(flags));

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

//    if (_counterPing > COUNTER_PING_COUNT)
//    {
//        Log::write(" WARRNING: ping timeout",
//                   Log::Write_Flag::STDOUT, "");
//        disconnectFromHost();
//    }
}

void Client::queryTech(tech_type_t type, char *data, uint64_t dataSize)
{
    char outData[BUFSIZ];
    auto size = ::queryTech(outData, BUFSIZ, data, dataSize, type);

    write({outData, static_cast<int>(size)});
}

void Client::responceIdentification(const Header *header)
{
    Q_ASSERT(header != NULL);

    //Пустой список устройств
    if (header->pkg == NULL)
    {
        _devices.clear();
        return;
    }

    const struct Identification *pkg = static_cast<const struct Identification *>(header->pkg);
    QString name(QByteArray{pkg->name, pkg->nameSize});

    iotv_obj_t *iot = createIotFromHeaderIdentification(header);

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
            connect(&device, &Device::signalQueryLogData, this, &Client::slotQuerLogData, Qt::QueuedConnection);

            connect(&device, &Device::signalOpenReadStream, this, &Client::slotOpenReadStream, Qt::QueuedConnection);
            connect(&device, &Device::signalCloseReadStream, this, &Client::slotCloseReadStream, Qt::QueuedConnection);
        }
    }
    else
        _devices[name].update(iot);

    clear_iotv_obj(iot);
}

void Client::responceState(const header_t *header)
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

void Client::responceRead(const header_t *header)
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
    {
        if (pkg->channelNumber == 15)
        {
//            auto res = QByteArray{pkg->data, static_cast<int>(pkg->dataSize)};
            _devices[name].setData(channel, {pkg->data, static_cast<int>(pkg->dataSize)});
        }
        _devices[name].setData(channel, {pkg->data, static_cast<int>(pkg->dataSize)});
    }
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
//        qDebug() << "PKG total: " << data.size();
        emit _devices[name].signalDataPkgComplete(channel, std::move(data));
    }
}

void Client::responceWrite(const header_t *header) const
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    // Нет реакции на ответ о записи
}

void Client::responcePingPoing(const header_t *header)
{
    Q_ASSERT(header != NULL);
    _counterPing = 0;
}

void Client::responceTech(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct Tech *pkg = static_cast<const struct Tech*>(header->pkg);
    if (pkg == nullptr)
        return;

    if (pkg->type == Tech_TYPE_EV_AC)
    {
        QByteArray data(reinterpret_cast<const char *>(pkg->data), pkg->dataSize);

        std::forward_list<const Base_Host *> hosts;
        for(const auto &pair : _devices)
            hosts.push_front(&pair.second);

        auto pairs = Event_Action_Parser::parseJson(data, hosts);
        auto event_vec = pairs.first.first;
        auto action_vec = pairs.first.second;
        auto event_groups = pairs.second.first;
        auto action_groups = pairs.second.second;

        _eventManager.reset();
        _eventManager = std::make_unique<IOTV_Event_Manager>(event_vec, action_vec, event_groups, action_groups);

//        for (const auto &el : event_groups) {
//            qDebug() << el;
//        }
        emit signalEventAction();
    }
}

void Client::responceLogData(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct Log_Data *pkg = static_cast<const struct Log_Data *>(header->pkg);
    if (pkg == nullptr)
        return;

    QString name(QByteArray{pkg->name, pkg->nameSize});

    if (!_devices.contains(name))
        return;

    if (header->fragment == 1)
        _devices[name].clearDataLog(pkg->channelNumber);

    if (pkg->dataSize > 0)
    {
//        QApplication::processEvents(QEventLoop::AllEvents);
        _devices[name].addDataLog(pkg->channelNumber, {pkg->data, pkg->dataSize});
    }

    if (header->fragment == header->fragments)
    {
//        qDebug() << "responceLogData" << pkg->channelNumber << QTime::currentTime();

        _devices[name].dataLogToPoints(pkg->channelNumber, pkg->flags);
        qDebug() << "channel " << pkg->channelNumber << "data size - " << "stop fragment";
    }

    // Что бы не вис графический интерфейс
//    static int counter = 0;
//    if (counter++ > 10)
//    {
//        QApplication::processEvents(QEventLoop::EventLoopExec);
//        counter = 0;
//    }

    // На стороне сервера поток Client занят отправкой данных и не отвечает на другие запросы от данного клиента
    // в том числе на пинг, что бы данный клиент не прирвал связь по пинг таймеру, обнуляем счетчик.
//    _counterPing = 0;
}

void Client::slotReciveData()
{
//     qDebug() << Q_FUNC_INFO << QTime::currentTime();

    _recivedBuff += _socket.readAll();

    bool error;
    uint64_t cutDataSize, expectedDataSize;

    while (_recivedBuff.size() > 0)
    {
        header_t* header = createPkgs(reinterpret_cast<uint8_t*>(_recivedBuff.data()), _recivedBuff.size(), &error, &expectedDataSize, &cutDataSize);

        if (error == true)
        {
            _recivedBuff.clear();
            clearHeader(header);
            break;
        }

        // Пакет не ещё полный
        if (expectedDataSize > 0)
        {
            clearHeader(header);
            break;
        }

//        if (header->assignment == HEADER_ASSIGNMENT_LOG_DATA)
//            qDebug() << "log data header" <<  QTime::currentTime();

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
            else if (header->assignment == HEADER_ASSIGNMENT_LOG_DATA)
            {
//                qDebug() << "responceLogData" << QTime::currentTime();
                responceLogData(header);
            }
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

void Client::slotQuerLogData(uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, log_data_flag_t flags)
{
//    qDebug() << "slotQuerLogData" << channelNumber << QTime::currentTime();

    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    queryLogDataHost(dev->getName(), startInterval, endInterval, interval, channelNumber, flags);
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
