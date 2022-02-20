#pragma once

#include <QTcpSocket>
#include <QHostAddress>
#include <memory>
#include <iostream>

#include "base_conn_type.h"
#include "../lib/Log/log.h"

#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 2021

class TCP_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    TCP_conn_type(const QString& name, const QString& address = DEFAULT_ADDRESS, quint16 port = DEFAULT_PORT,
                       Base_conn_type *parent = nullptr);

    quint16 getPort() const;
    void setPort(quint16 port);

    virtual qint64 write(const QByteArray &data) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;

protected:
    virtual QByteArray readAll() override;

private:
    std::unique_ptr<QTcpSocket> _tcpSocket;
    quint16 _tcpPort;

private slots:
    void slotNewConnection();
    void slotSocketDisconnected();

//    void slotReadData();
    void slotError(QAbstractSocket::SocketError error);
};
