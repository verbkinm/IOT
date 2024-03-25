#pragma once

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

#include "base_conn_type.h"

class TCP_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    TCP_conn_type(const QString& name, const QString& address, quint16 port, QObject *parent);

    virtual qint64 write(const QByteArray &data, qint64 size = -1) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;

    static constexpr int SERVER_RECONNECT_INTERVAL = 1000; // таймер неудавшегося подключения


protected:
    virtual QByteArray readAll() override;

private:
    QTcpSocket _tcpSocket;
    quint16 _tcpPort;

private slots:
    void slotNewConnection();
    void slotSocketDisconnected();
    void slotSocketStateChanged(QAbstractSocket::SocketState socketState);

    void slotError(QAbstractSocket::SocketError error);
};
