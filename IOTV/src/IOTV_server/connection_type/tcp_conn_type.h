#pragma once

#include <QTcpSocket>
#include <QHostAddress>
#include <memory>
#include <iostream>

#include "base_conn_type.h"
#include "log.h"

class TCP_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    TCP_conn_type(const QString& name, const QString& address, quint16 port, QObject *parent);

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
    void slotSocketStateChanged(QAbstractSocket::SocketState socketState);

    void slotError(QAbstractSocket::SocketError error);
};
