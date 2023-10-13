#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

#define BUFSIZE 8

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZE];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = HEADER_SIZE;
uint64_t cutDataSize = 0;
bool error = false;


uint8_t readType[3] = {DATA_TYPE_FLOAT_32, DATA_TYPE_FLOAT_32, DATA_TYPE_FLOAT_32};
//uint8_t writeType[3] = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL};

QByteArray buffer;

struct IOTV_Server_embedded iot = {
    .id = 2,
    .name = "bme-280",
    .description = "Главней всего, погода в доме",
    .numberReadChannel = 3,
    .readChannel = NULL,
    .readChannelType = readType,
    .numberWriteChannel = 0,
    .writeChannelType = NULL,
    .state = 1,
    .nameSize = static_cast<uint8_t>(strlen(iot.name)),
    .descriptionSize = static_cast<uint8_t>(strlen(iot.description)),
};


void slotDataRecived()
{
    buffer += socket->readAll();

    qDebug() << buffer.toHex(':') << '\n';

    if ((uint64_t)buffer.size() < expextedDataSize)
        return;

    while (buffer.size() > 0)
    {
        auto bufSize = (buffer.size() > BUFSIZE) ? BUFSIZE : buffer.size();
        memcpy(recivedBuffer, buffer.data(), bufSize);

        struct Header* header = createPkgs((uint8_t*)recivedBuffer, bufSize, &error, &expextedDataSize, &cutDataSize);

        if (error == true)
        {
            realBufSize = 0;
            expextedDataSize = 0;
            cutDataSize = 0;
            buffer.clear();
            return;
        }

        if (expextedDataSize > 0)
            return;

        if (header->type == HEADER_TYPE_REQUEST)
        {
            if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
            {
                uint64_t size = responseIdentificationData(transmitBuffer, BUFSIZE, &iot);
                socket->write(transmitBuffer, size);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
            {
                uint64_t size = responseReadData(transmitBuffer, BUFSIZE, &iot, header);
                socket->write(transmitBuffer, size);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
            {
                uint64_t size = responseWriteData(transmitBuffer, BUFSIZE, &iot, header);
                socket->write(transmitBuffer, size);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_PING_PONG)
            {
                uint64_t size = responsePingData(transmitBuffer, BUFSIZE);
                socket->write(transmitBuffer, size);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_STATE)
            {
                uint64_t size = responseStateData(transmitBuffer, BUFSIZE, &iot);
                socket->write(transmitBuffer, size);
            }
        }

        buffer = buffer.mid(cutDataSize);
    }

    //    //!!!
    ////    memmove((void*)recivedBuffer, (void*)&recivedBuffer[cutDataSize], BUFSIZE - cutDataSize);
    //    realBufSize -= cutDataSize; // тут всегда должно уходить в ноль, если приём идёт по 1 байту!

    //    //страховка
    //    if (realBufSize >= BUFSIZE)
    //    {
    //        realBufSize = 0;
    //        expextedDataSize = 0;
    //        cutDataSize = 0;
    //    }

}
//для ПК
void slotDisconnected()
{
    QString strOut = "disconnected from " + socket->peerAddress().toString()
            + ":" + QString::number(socket->peerPort());
    std::cout << strOut.toStdString() << std::endl;

    buffer.clear();
    error = false;
    cutDataSize = 0;
    expextedDataSize = 0;
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

    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 3);

    iot.readChannel[0].dataSize = dataSizeonDataType(readType[0]);
    iot.readChannel[1].dataSize = dataSizeonDataType(readType[1]);
    iot.readChannel[2].dataSize = dataSizeonDataType(readType[2]);

    iot.readChannel[0].data = (char *)malloc(iot.readChannel[0].dataSize);
    float value = -10.7;
    memcpy(iot.readChannel[0].data, &value, iot.readChannel[0].dataSize);

    iot.readChannel[1].data = (char *)malloc(iot.readChannel[1].dataSize);
    value = 530.2323;
    memcpy(iot.readChannel[1].data, &value, iot.readChannel[1].dataSize);

    iot.readChannel[2].data = (char *)malloc(iot.readChannel[2].dataSize);
    value = 97.5;
    memcpy(iot.readChannel[2].data, &value, iot.readChannel[2].dataSize);

    server = new QTcpServer;

    QObject::connect(server, &QTcpServer::newConnection, [&](){
        slotNewConnection();
    });

    server->listen(QHostAddress("127.0.0.1"), 2025);
    std::cout << "Start service on 127.0.0.1:2025" << std::endl;

    return a.exec();
}
