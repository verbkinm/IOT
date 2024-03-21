#pragma once

#include "base_conn_type.h"
#include <QTcpSocket>

class TCP_REVERSE_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    TCP_REVERSE_conn_type(const QString& name, QTcpSocket *socket, QObject *parent);

public:
    virtual qint64 write(const QByteArray &data, qint64 size) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;

protected:
    virtual QByteArray readAll() override;

private:
    QTcpSocket *_tcpSocket;
};

