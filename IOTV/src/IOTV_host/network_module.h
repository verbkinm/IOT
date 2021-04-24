#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

#include <QTcpServer>
#include <QTcpSocket>

#include "iot_server.h"
#include "protocol_class.h"

class Network_Module : public QTcpServer
{
    Q_OBJECT
public:
    Network_Module();

private:
    void response(QTcpSocket *socket, QByteArray &data);
    void postResponse(QTcpSocket *socket, QByteArray &data);

    IOT_Server iotServer;

private slots:
    void slotNewConnection();
    void slotDataRecived();

    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError error);
};

#endif // NETWORK_MODULE_H
