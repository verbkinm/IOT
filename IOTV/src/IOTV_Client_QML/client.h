#pragma once

#include <map>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QFileInfo>
#include <QEvent>

#include "log.h"
#include "IOTV_SH.h"
#include "device.h"

class Client : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint16 totalDevice READ countDevices NOTIFY countDeviceChanged)
    Q_PROPERTY(quint16 onlineDevice READ countDeviceOnline NOTIFY onlineDeviceChanged)
    Q_PROPERTY(bool state READ stateConnection NOTIFY stateConnectionChanged)
public:
    Client(QObject *parent = nullptr);
    ~Client();

//    qint64 writeData(const QByteArray &data);

    int countDevices() const;
    int countDeviceOnline() const;

    QByteArray readData(const QString &deviceName, uint8_t channelNumber) const;

    Q_INVOKABLE QList<QObject*> devList();
    Q_INVOKABLE QObject *deviceByName(QString name);

    bool stateConnection() const;

private:
    static constexpr int TIME_OUT = 5000;
    QTcpSocket _socket;
    QByteArray _recivedBuff;

    QTimer _timerPing;                      // "Пингуем" сервер

    bool _stateConnection;                  // Состояние подключения к серверу.
    uint64_t _expectedDataSize;

    //!!! unorder_map
    std::map<QString, Device> _devices;

    void queryIdentification();
    void queryState(const QString &name);
    void queryRead(const QString &name, uint8_t channelNumber);
    void queryWrite(const QString &name, uint8_t channelNumber, const QByteArray &data);
    void queryPingPoing();

    void responceIdentification(const struct Header *header);
    void responceState(const struct Header *header);
    void responceRead(const struct Header* header);
    void responceWrite(const struct Header *header) const;
    void responcePingPoing(const struct Header *header);

    void write(const QByteArray &data);

    void setStateConnection(bool newStateConnection);

//    bool ifDevIdZeroQueryIdentification(const QString &name);

public slots:
    void connectToHost(const QString &address, qint64 port);
    void disconnectFromHost();

    void slotConnected();
    void slotDisconnected();
    void slotStateChanged(QAbstractSocket::SocketState socketState);

private slots:
    void slotReciveData();

    void slotQueryDevList();
    void slotQueryRead();
    void slotQueryState();
    void slotQueryWrite(int channelNumber, QByteArray data);

    void slotError(QAbstractSocket::SocketError error);
//    void slotConnectWait();

signals:
    void signalConnected();
    void signalConnecting();
    void signalDisconnected();

    void countDeviceChanged();
    void onlineDeviceChanged();
    void stateConnectionChanged();
    void autoConnectChanged();
    void slotPing();
//    void signalConnectWait();
};
