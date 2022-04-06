#pragma once

#include "base_conn_type.h"
#include <QUdpSocket>
#include "log.h"
#include "IOTV_SH.h"

class Udp_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
     Udp_conn_type(const QString& name, const QString& address = "127.0.0.1", quint16 port = 2021, QObject *parent = nullptr);

     // Base_conn_type interface
public:
     virtual qint64 write(const QByteArray &data) override;
     virtual void connectToHost() override;
     virtual void disconnectFromHost() override;
};
