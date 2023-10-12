#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

#include "widget.h"

Widget *camera;
char imageBuffer[1024 * 5];

#define BUFSIZE 1024 * 5

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;

char recivedBuffer[BUFSIZE], transmitBuffer[BUFSIZE];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = HEADER_SIZE;
uint64_t cutDataSize = 0;
bool error = false;


uint8_t readType[1] = {DATA_TYPE_RAW};
uint8_t writeType[3] = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL};

QByteArray buffer;

struct IOTV_Server_embedded iot = {
    .id = 8,
    .name = "Device",
    .description = "Description",
    .numberReadChannel = 1,
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
        memcpy(recivedBuffer, buffer.data(), buffer.size());

        struct Header* header = createPkgs((uint8_t*)recivedBuffer, buffer.size(), &error, &expextedDataSize, &cutDataSize);

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
//                iot.readChannel[0].dataSize = camera->getData(iot.readChannel[0].data, sizeof(imageBuffer));
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
                uint64_t size = responseStateData(transmitBuffer, BUFSIZ, &iot);
                socket->write(transmitBuffer, size);
            }
        }

        buffer = buffer.mid(cutDataSize);
    }

    //    //!!!
    ////    memmove((void*)recivedBuffer, (void*)&recivedBuffer[cutDataSize], BUFSIZ - cutDataSize);
    //    realBufSize -= cutDataSize; // тут всегда должно уходить в ноль, если приём идёт по 1 байту!

    //    //страховка
    //    if (realBufSize >= BUFSIZ)
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

    camera = new Widget;

    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 3);

    iot.readChannel[0].dataSize = sizeof(imageBuffer);//dataSizeonDataType(readType[0]);
    iot.readChannel[0].data = imageBuffer;

    server = new QTcpServer();

    QObject::connect(server, &QTcpServer::newConnection, [&](){
        slotNewConnection();
    });

    server->listen(QHostAddress("127.0.0.1"), 2028);
    std::cout << "Start service on 127.0.0.1:2028" << std::endl;

    return a.exec();
}
