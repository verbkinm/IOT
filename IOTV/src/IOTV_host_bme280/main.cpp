#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>

#include "log.h"
#include "Protocols/IOTV_SH.h"
#include "iot_server.h"

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;
char resivedBuffer[BUFSIZ] {0};
IOT_Server iot;

void slotDataRecived()
{
//    resivedBuffer +=
    socket->read(resivedBuffer, 1);

//    Log::write("data recived form " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort())
//               + " <- " + resivedBuffer.toHex(':'));
Mark:
//    if (resivedBuffer.isEmpty())
//        return;

    char *data = nullptr;
    uint16_t dataSize = 0;

    if (resivedBuffer[0] == Protocol_class::QUERY_WAY_BYTE)
    {
        dataSize = Protocol_class::response_WAY(iot, data);
        socket->write(data, dataSize);
        memmove((void*)resivedBuffer, (void*)&resivedBuffer[1], BUFSIZ - 1);
    }

    else if ((resivedBuffer[0] & 0x0F) == Protocol_class::QUERY_READ_BYTE)
    {
        iot.newValue();

        uint8_t channelNumber = resivedBuffer[0] >> 4;
        dataSize = Protocol_class::response_READ(iot, channelNumber, data);

        socket->write(data, dataSize);
        memmove((void*)resivedBuffer, (void*)&resivedBuffer[1], BUFSIZ - 1);
    }
    else if (resivedBuffer[0] == Protocol_class::QUERY_PING_BYTE)
    {
        dataSize = Protocol_class::response_Pong(data);
        socket->write(data, dataSize);
        memmove((void*)resivedBuffer, (void*)&resivedBuffer[1], BUFSIZ - 1);
    }
//    else
//        resivedBuffer.clear();

//    if (data != nullptr)
//        delete[] data;

//    if (resivedBuffer.size() > 0)
//        goto Mark; //!!!
}

void slotDisconnected()
{
    QString strOut = "disconnected from " + socket->peerAddress().toString()
            + ":" + QString::number(socket->peerPort());
    Log::write(strOut);
}

void slotNewConnection()
{
    socket = server->nextPendingConnection();
    Log::write("new connection: "+ socket->peerAddress().toString() + QString::number(socket->peerPort()));

    QObject::connect(socket, &QTcpSocket::readyRead, slotDataRecived);
    QObject::connect(socket, &QTcpSocket::disconnected, slotDisconnected);
    QObject::connect(socket, &QTcpSocket::disconnected, &QObject::deleteLater);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    server = new QTcpServer;

    QObject::connect(server, &QTcpServer::newConnection, [&](){
        slotNewConnection();
    });


    server->listen(QHostAddress("127.0.0.1"), 2023);
    Log::write("Start service on 127.0.0.1:2023");

    return a.exec();
}
