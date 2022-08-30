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

public:
    Client() = delete;
    Client(const QString &address, const quint16 &port, QObject *parent = nullptr);
    ~Client();

    void connectToHost();
    void disconnectFromHost();

    qint64 writeData(const QByteArray &data);

    const QString &address() const;
    quint16 port() const;

    QAbstractSocket::SocketState connectionState() const;
    int countDevices() const;
    int countDeviceOnline() const;
    std::set<QString> deviceList() const;

    QByteArray readData(const QString &deviceName, uint8_t channelNumber) const;

private:
    QTcpSocket _socket;
    QByteArray _recivedBuff;

    QTimer _timerDevList;

    const QString _address;
    const quint16 _port;

    QTimer _reconnectTimer;

    std::unordered_map<QString, Device> _devices;

    void response_DEV_LIST(IOTV_SC::RESPONSE_PKG *pkg);
    void response_STATE(IOTV_SC::RESPONSE_PKG *pkg);
    void response_READ(IOTV_SC::RESPONSE_PKG *pkg);
    void response_WRITE(IOTV_SC::RESPONSE_PKG *pkg) const;

    void write(const QByteArray &data);

private slots:
    void slotConnected();
    void slotDisconnected();

    void slotReciveData();

    void slotQueryRead();
    void slotQueryState();
    void slotQueryDevList();

    void slotError(QAbstractSocket::SocketError error);

signals:
    void signalConnected();
    void signalDisconnected();

    void addressChanged();
    void portChanged();
};
