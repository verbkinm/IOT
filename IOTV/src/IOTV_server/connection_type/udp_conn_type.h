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

    virtual qint64 write(const QByteArray &data, qint64 size = -1) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;

protected:
    virtual QByteArray readAll() override;

private:
    QUdpSocket _udpSocket;
    quint16 _udpPort;

private slots:
    void slotError(QAbstractSocket::SocketError error);
    void slotSocketStateChange(QAbstractSocket::SocketState socketState);
    //    void slotNewConnection();
    //    void slotSocketDisconnected();
    //    void slotSocketStateChanged(QAbstractSocket::SocketState socketState);

    //    void slotError(QAbstractSocket::SocketError error);
};
