#pragma once

#include "base_conn_type.h"
#include <QUdpSocket>
#include "log.h"
//#include "IOTV_SH.h"

class Udp_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
     Udp_conn_type(const QString& name, const QString& address, quint16 port, QObject *parent);

public:
     virtual qint64 write(const QByteArray &data, qint64 size = -1) override;
     virtual void connectToHost() override;
     virtual void disconnectFromHost() override;
};
