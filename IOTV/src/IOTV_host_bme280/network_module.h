#pragma once

#include <QTcpServer>
#include <QTcpSocket>

#include "iot_server.h"
#include "Protocols/IOTV_SH.h"

class Network_Module : public QTcpServer
{
    Q_OBJECT
public:
    Network_Module();

private:
    IOT_Server iotServer;

private slots:
    void slotNewConnection();
    void slotDataRecived();

    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError error);
};
