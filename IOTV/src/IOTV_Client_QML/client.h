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

    Q_INVOKABLE QList<QObject*> devList();
    Q_INVOKABLE QObject *deviceByName(const QString &name);
    Q_INVOKABLE void queryEventAction();
    Q_INVOKABLE QList<QList<QVariantMap>> evAcList() const;
    Q_INVOKABLE void saveEventAction(QVariantMap event, QVariantMap action, QString oldName);
    Q_INVOKABLE void removeEventAction(QString name);

    bool stateConnection() const;

private:
    QTcpSocket _socket;
    QByteArray _recivedBuff;

    QTimer _timerPing;
    // Что бы не плодить таймеры. Если отправляется пакет пинг уже N-ый раз, значит ответов не было и соединение разрывается
    static constexpr int COUNTER_PING_COUNT = 5;
    // Если будет превышено кол-во попыток получения от сервера ответа на пинг запрос, клиент (данное приложение) отключается от сервера
    int _counterPing;

    //!!! unorder_map
    std::map<QString, Device> _devices;

    QList<QList<QVariantMap>> _evAcList;

    void queryIdentification();
    void queryState(const QString &name);
    void queryRead(const QString &name, uint8_t channelNumber);
    void queryLogDataHost(const QString &name, uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, LOG_DATA_FLAGS flags);
    void queryWrite(const QString &name, uint8_t channelNumber, const QByteArray &data);
    void queryPing();
    void queryTech(Tech_TYPE type, char *data, uint64_t dataSize);

    void responceIdentification(const struct Header *header);
    void responceState(const struct Header *header);
    void responceRead(const struct Header* header);
    void responceReadStream(const struct Header *header);
    void responceWrite(const struct Header *header) const;
    void responcePingPoing(const struct Header *header);
    void responceTech(const struct Header *header);
    void responceLogData(const struct Header *header);

    qint64 write(const QByteArray &data);

    QList<QList<QVariantMap>> replaceRealNameToAlias(const QList<QList<QVariantMap>> &evActList) const;
    QList<QList<QVariantMap>> replaceAliasToRealName(const QList<QList<QVariantMap>> &evActList) const;
    QString findRealName(const QString &alias) const;
    QString findAliasName(const QString &realName) const;

    void removeEventAction(QList<QList<QVariantMap> > &list, const QString &name);

    std::forward_list<const Base_Host *> host_list() const;
    std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>> convert(const QList<QList<QVariantMap>> &list) const;
    void clearList(std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *> > > &list) const;

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
    void slotQuerLogData(uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, LOG_DATA_FLAGS flags);

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
