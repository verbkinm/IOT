#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>

#include "log.h"
#include "Protocols/IOTV_SH.h"
#include "iot_server.h"

QTcpServer server;
QTcpSocket* socket = nullptr;
QByteArray buffer, resivedBuffer;
IOT_Server iot;

void slotDataRecived()
{
    resivedBuffer += socket->readAll();

    Log::write("data recived form " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort())
               + " <- " + resivedBuffer.toHex(':'));

Mark:
    if (resivedBuffer.isEmpty())
        return;

    if (resivedBuffer.at(0) == Protocol_class::QUERY_WAY_BYTE)
    {
        socket->write(Protocol_class::response_WAY(iot));
        resivedBuffer = resivedBuffer.mid(1);
    }

    else if ((resivedBuffer.at(0) & 0x0F) == Protocol_class::QUERY_READ_BYTE)
    {
        socket->write(Protocol_class::response_READ(iot, resivedBuffer));
        resivedBuffer = resivedBuffer.mid(1);
    }
    else if (resivedBuffer.at(0) == Protocol_class::QUERY_PING_BYTE)
    {
        socket->write(Protocol_class::response_Pong());
        resivedBuffer = resivedBuffer.mid(1);
    }
    else
        resivedBuffer.clear();

    if (resivedBuffer.size() > 0)
        goto Mark;
}

void slotDisconnected()
{
    QString strOut = "disconnected from " + socket->peerAddress().toString()
            + ":" + QString::number(socket->peerPort());
    Log::write(strOut);
}

void slotNewConnection()
{
    socket = server.nextPendingConnection();
    Log::write("new connection: "+ socket->peerAddress().toString() + QString::number(socket->peerPort()));

    QObject::connect(socket, &QTcpSocket::readyRead, slotDataRecived);
    QObject::connect(socket, &QTcpSocket::disconnected, slotDisconnected);
    QObject::connect(socket, &QTcpSocket::disconnected, &QObject::deleteLater);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QObject::connect(&server, &QTcpServer::newConnection, [&](){
        slotNewConnection();
    });


    server.listen(QHostAddress("127.0.0.1"), 2023);
    Log::write("Start service on 127.0.0.1:2023");

    return a.exec();
}
