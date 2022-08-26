#pragma once

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

#include "log.h"
#include "IOTV_SC.h"

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

    QString _address;
    quint16 _port;

    QTimer _reconnectTimer;

    uint8_t _reconnectTimerTrying;

private slots:
    void slotConnected();
    void slotDisconnected();

    void slotReadData();
    void slotError(QAbstractSocket::SocketError error);

signals:
    void signalConnected();
    void signalDisconnected();

    void addressChanged();
    void portChanged();
};
