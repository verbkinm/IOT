#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>

//#include "Protocols/IOTV_SH.h"
#include "Protocols/embedded/creatorpkgs.h"
#include "iot_server.h"

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;

IOTV_Server iot = {
    .id = 1,
    .name = "Demo name",
    .description = "Switch",
    .readChannel = {false, false, true},
    .readChannelType = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL},
    .writeChannelType = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL}
};

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = HEADER_SIZE;
uint64_t cutDataSize = 0;
bool error = false;



void slotDataRecived()
{
    socket->read(&recivedBuffer[realBufSize], 1);
    ++realBufSize;

    if (realBufSize < expextedDataSize)
        return;

    struct Header* header = createPkgs((uint8_t*)recivedBuffer, realBufSize, &error, &expextedDataSize, &cutDataSize);

    if (error == true)
    {
        realBufSize = 0;
        expextedDataSize = 0;
        cutDataSize = 0;
        return;
    }

    if (expextedDataSize > 0)
        return;

    if (header->type == Header::HEADER_TYPE_REQUEST)
    {
        if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
        {
            uint64_t size = responseIdentificationData(transmitBuffer, BUFSIZ, &iot);
            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == Header::HEADER_ASSIGNMENT_READ)
        {
            uint64_t size = responseReadData(transmitBuffer, BUFSIZ, &iot, header);
            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
        {

        }
        else if(header->assignment == Header::HEADER_ASSIGNMENT_PING_PONG)
        {
            uint64_t size = responsePingData(transmitBuffer, BUFSIZ);
            socket->write(transmitBuffer, size);
        }
    }

    //!!!
//    memmove((void*)recivedBuffer, (void*)&recivedBuffer[cutDataSize], BUFSIZ - cutDataSize);
    realBufSize -= cutDataSize; // тут всегда должно уходить в ноль, если приём идёт по 1 байту!

    //страховка
    if (realBufSize >= BUFSIZ)
    {
        realBufSize = 0;
        expextedDataSize = 0;
        cutDataSize = 0;
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

    server->listen(QHostAddress("127.0.0.1"), 2024);
    std::cout << "Start service on 127.0.0.1:2024" << std::endl;

    return a.exec();
}
