#pragma once

#include <unordered_map>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <set>

#include "log.h"
#include "IOTV_SC.h"
#include "device.h"

class Client : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString addr READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(quint16 totalDevice READ countDevices NOTIFY countDeviceChanged)
    Q_PROPERTY(quint16 onlineDevice READ countDeviceOnline NOTIFY onlineDeviceChanged)

public:
    Client(QObject *parent = nullptr);
    ~Client();

    qint64 writeData(const QByteArray &data);

    const QString &address() const;
    quint16 port() const;
    QAbstractSocket::SocketState connectionState() const;
    int countDevices() const;
    int countDeviceOnline() const;
    std::set<QString> deviceList() const;

    void setAddress(const QString &address);
    void setPort(quint64 port);

    QByteArray readData(const QString &deviceName, uint8_t channelNumber) const;

    Q_INVOKABLE QList<QObject*> devList();

private:
    QTcpSocket _socket;
    QByteArray _recivedBuff;

    QTimer _timerDevList;
    QTimer _reconnectTimer;

    QString _address;
    quint16 _port;

    std::unordered_map<QString, Device> _devices;

    void response_DEV_LIST(IOTV_SC::RESPONSE_PKG *pkg);
    void response_STATE(IOTV_SC::RESPONSE_PKG *pkg);
    void response_READ(IOTV_SC::RESPONSE_PKG *pkg);
    void response_WRITE(IOTV_SC::RESPONSE_PKG *pkg) const;

    void write(const QByteArray &data);

public slots:
    void connectToHost();
    void disconnectFromHost();

    void slotConnected();
    void slotDisconnected();

private slots:
    void slotReciveData();

    void slotQueryRead();
    void slotQueryState();
    void slotQueryDevList();

    void slotError(QAbstractSocket::SocketError error);

signals:
    void signalConnected();
    void signalDisconnected();

    void addressChanged(QString);
    void portChanged(quint64);

    void countDeviceChanged();
    void onlineDeviceChanged();
};
