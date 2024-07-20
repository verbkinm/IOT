#pragma once

#include <map>
#include <forward_list>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QFileInfo>
#include <QEvent>

#include <QChartView>


#include "actions/iotv_action.h"
#include "device.h"
#include "events/iotv_event.h"
#include "iotv_event_manager.h"

class Client : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint16 totalDevice READ countDevices NOTIFY countDeviceChanged)
    Q_PROPERTY(quint16 onlineDevice READ countDeviceOnline NOTIFY onlineDeviceChanged)
    Q_PROPERTY(bool state READ stateConnection NOTIFY stateConnectionChanged)

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    int countDevices() const;
    int countDeviceOnline() const;

    QByteArray readData(const QString &deviceName, uint8_t channelNumber) const;

    Q_INVOKABLE QList<QObject *> devList();
    Q_INVOKABLE QList<QString> allHostAliasName() const;  // список имён хостов, который включает в себя так же и имена хостов из файла IOTV_Event_Action.json
    Q_INVOKABLE QObject *deviceByName(const QString &name);
    Q_INVOKABLE void queryEventAction();

    // Events
    Q_INVOKABLE void saveEvent(IOTV_Event *event);
    Q_INVOKABLE void saveEventGroup(const QString &groupName);

    Q_INVOKABLE void removeEvent(IOTV_Event *event);
    Q_INVOKABLE void removeEvent(const QString &groupName, const QString &eventName);
    Q_INVOKABLE void removeEventGroup(const QString &groupName);

    Q_INVOKABLE void renameEventGroup(const QString &oldGroupName, const QString &newGroupName);

    Q_INVOKABLE bool isEmptyEventGroup(const QString &groupName); // пуста ли группа
    Q_INVOKABLE bool isExistsEventGroup(const QString &groupName); // есть ли группа событий с таким именем
    Q_INVOKABLE bool isExistsEventNameInGroup(const QString &groupName, const QString &eventName);

    Q_INVOKABLE QList<QString> eventsGroupList() const;
    Q_INVOKABLE QList<QString> eventsListInGroup(const QString &groupName) const;

    Q_INVOKABLE IOTV_Event *copyEventByNameAndGroup(const QString &eventName, const QString &groupName) const;
    Q_INVOKABLE IOTV_Event *createEmptyEvent(const QString &eventType, const QString &eventName, const QString &groupName) const;

    Q_INVOKABLE void runEvent(const QString &groupName, const QString &eventName);

    // Actions
    Q_INVOKABLE void saveAction(IOTV_Action *action);
    Q_INVOKABLE void saveActionGroup(const QString &groupName);

    Q_INVOKABLE void removeAction(const QString &groupName, const QString &eventName);
    Q_INVOKABLE void removeActionGroup(const QString &groupName);

    Q_INVOKABLE void renameActionGroup(const QString &oldGroupName, const QString &newGroupName);

    Q_INVOKABLE bool isEmptyActionGroup(const QString &groupName); // пуста ли группа
    Q_INVOKABLE bool isExistsActionGroup(const QString &groupName); // есть ли группа событий с таким именем
    Q_INVOKABLE bool isExistsActionNameInGroup(const QString &groupName, const QString &eventName);

    Q_INVOKABLE QList<QString> actionsGroupList() const;
    Q_INVOKABLE QList<QString> actionsListInGroup(const QString &groupName) const;

    Q_INVOKABLE IOTV_Action *copyActionByNameAndGroup(const QString &actionName, const QString &groupName) const;
    Q_INVOKABLE IOTV_Action *createEmptyAction(const QString &actionType, const QString &actionName, const QString &groupName) const;

    Q_INVOKABLE void runAction(const QString &groupName, const QString &actiontName);


    // используется в qml для удаления временной копии события или действия
    Q_INVOKABLE void deleteObject(QObject *obj) const;

    //
//    Q_INVOKABLE QString realName(const QString &devName) const;
//    Q_INVOKABLE QString aliasName(const QString &devName) const;


    bool stateConnection() const;

private:
    QTcpSocket _socket;
    QByteArray _recivedBuff;

    QTimer _timerPing;
    // Что бы не плодить таймеры. Если отправляется пакет пинг уже N-ый раз, значит ответов не было и соединение разрывается
    static constexpr int COUNTER_PING_COUNT = 60;
    // Если будет превышено кол-во попыток получения от сервера ответа на пинг запрос, клиент (данное приложение) отключается от сервера
    int _counterPing;

    //!!! unorder_map
    std::map<QString, Device> _devices;

    std::unique_ptr<IOTV_Event_Manager> _eventManager;
    void saveEventAction();

    void queryIdentification();
    void queryState(const QString &name);
    void queryRead(const QString &name, uint8_t channelNumber);
    void queryLogDataHost(const QString &name, uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, log_data_flag_t flags);
    void queryWrite(const QString &name, uint8_t channelNumber, const QByteArray &data);
    void queryPing();
    void queryTech(tech_type_t type, char *data, uint64_t dataSize);

    void responceIdentification(const header_t *header);
    void responceState(const header_t *header);
    void responceRead(const header_t* header);
    void responceReadStream(const header_t *header);
    void responceWrite(const header_t *header) const;
    void responcePingPoing(const header_t *header);
    void responceTech(const header_t *header);
    void responceLogData(const header_t *header);

    qint64 write(const QByteArray &data);

public slots:
    void connectToHost(const QString &address, qint64 port);
    void disconnectFromHost();

    void slotConnected();
    void slotDisconnected();
    void slotStateChanged(QAbstractSocket::SocketState socketState);

    void slotOpenReadStream(int channel);
    void slotCloseReadStream(int channel);

private slots:
    void slotReciveData();

    void slotQueryIdentification();
    void slotQueryRead();
    void slotQueryState();
    void slotQueryWrite(int channelNumber, const QByteArray &data);
    void slotQuerLogData(uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, log_data_flag_t flags);

    void slotError(QAbstractSocket::SocketError error);

signals:
    void signalConnected();
    void signalConnecting();
    void signalDisconnected();

    void countDeviceChanged();
    void onlineDeviceChanged();
    void stateConnectionChanged();
    void autoConnectChanged();

    void signalEventAction();
};
