#pragma once

#include "base_conn_type.h"
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>

#include "base_conn_type.h"
#include "log.h"


class Udp_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    Udp_conn_type(const QString& name, const QString& address, quint16 port, QObject *parent);
    ~Udp_conn_type();

    virtual qint64 write(const char *data, qint64 size) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;

protected:
    virtual QByteArray readAll() override;

private:
    QUdpSocket *_udpSocket;
    quint16 _udpPort;

private slots:
    void slotError(QAbstractSocket::SocketError error);
    void slotSocketStateChange(QAbstractSocket::SocketState socketState);
};
