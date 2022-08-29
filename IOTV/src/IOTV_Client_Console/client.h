#pragma once

#include <unordered_map>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

#include "log.h"
#include "IOTV_SC.h"
#include "device.h"

class Client : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)

public:
    Client(const QString &address, const quint16 &port, QObject *parent = nullptr);

    ~Client();

    void connectToHost();
    void disconnectFromHost();

    qint64 writeData(const QByteArray &data);

    const QString &address() const;
    void setAddress(const QString &newAddress);

    quint16 port() const;
    void setPort(quint16 newPort);

private:
    QTcpSocket _socket;
    QByteArray _recivedBuff;

    QString _address;
    quint16 _port;

    QTimer _reconnectTimer;

    uint8_t _reconnectTimerTrying;

    std::unordered_map<QString, Device> _devices;

    void response_DEV_LIST(IOTV_SC::RESPONSE_PKG *pkg);
    void response_STATE(IOTV_SC::RESPONSE_PKG *pkg);
    void response_READ(IOTV_SC::RESPONSE_PKG *pkg);
    void response_WRITE(IOTV_SC::RESPONSE_PKG *pkg);

private slots:
    void slotConnected();
    void slotDisconnected();

    void slotReciveData();

    void slotQueryRead();
    void slotQueryState();

    void slotError(QAbstractSocket::SocketError error);

signals:
    void signalConnected();
    void signalDisconnected();

    void addressChanged();
    void portChanged();
};
