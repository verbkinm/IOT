#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>

#include "Protocols/IOTV_SH.h"
#include "iot_server.h"

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;

IOTV_Server iot;

char recivedBuffer[BUFSIZ] {0}, transmitBuffer[BUFSIZ] {0};
char* ptrBuf = recivedBuffer;


void slotDataRecived()
{
    uint16_t dataSize = 0;
    uint64_t recvSize = socket->read(ptrBuf, BUFSIZ); //!!!
    ptrBuf += recvSize;

    while(ptrBuf != recivedBuffer)
    {
        if (recivedBuffer[0] == Protocol_class::QUERY_WAY_BYTE)
        {
            dataSize = Protocol_class::response_WAY(iot, transmitBuffer);
            socket->write(transmitBuffer, dataSize);
            memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFSIZ - 1);
            ptrBuf--;
        }
        else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_READ_BYTE)
        {
            iot.newValue();

            dataSize = Protocol_class::response_READ(iot, recivedBuffer, transmitBuffer);

            socket->write(transmitBuffer, dataSize);
            memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFSIZ - 1);
            ptrBuf--;
        }
        else if (recivedBuffer[0] == Protocol_class::QUERY_PING_BYTE)
        {
            dataSize = Protocol_class::response_Pong(transmitBuffer);
            socket->write(transmitBuffer, dataSize);
            memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFSIZ - 1);
            ptrBuf--;
        }
        else
            ptrBuf = recivedBuffer;
    }
}
//для ПК
void slotDisconnected()
{
    QString strOut = "disconnected from " + socket->peerAddress().toString()
            + ":" + QString::number(socket->peerPort());
    std::cout << strOut.toStdString() << std::endl;
}

//для ПК
void slotNewConnection()
{
    socket = server->nextPendingConnection();
    QString strOut = "new connection: "+ socket->peerAddress().toString() + QString::number(socket->peerPort());
    std::cout << strOut.toStdString() << std::endl;

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
    std::cout << "Start service on 127.0.0.1:2023" << std::endl;

    return a.exec();
}
