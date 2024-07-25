#pragma once

#include "base_conn_type.h"
#include <QTcpSocket>

class TCP_REVERSE_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    TCP_REVERSE_conn_type(const QString& name, QTcpSocket *socket, QObject *parent);
    ~TCP_REVERSE_conn_type();

public:
    virtual qint64 write(const char *data, qint64 size) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;

protected:
    virtual QByteArray readAll() override;

private:
    QTcpSocket *_tcpSocket;

private slots:
    void slotSocketDisconnected();

    void slotSocketStateChanged(QAbstractSocket::SocketState socketState);
    void slotError(QAbstractSocket::SocketError error);
};

