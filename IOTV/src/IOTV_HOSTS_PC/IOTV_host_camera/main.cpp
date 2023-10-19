#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QBuffer>

#include <iostream>
#include <fstream>

#include "QtWidgets/qapplication.h"
#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

#include "iotv_types.h"
#include "widget.h"

Widget *camera;

//#define BUFSIZE

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = HEADER_SIZE;
uint64_t cutDataSize = 0;
bool error = false;


uint8_t readType[4] = {DATA_TYPE_RAW, DATA_TYPE_INT_16, DATA_TYPE_INT_16, DATA_TYPE_INT_16};
//uint8_t writeType[3] = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL};

QByteArray buffer;

struct IOTV_Server_embedded iot = {
    .id = 8,
    .numberReadChannel = 4,
    .numberWriteChannel = 0,
    .state = 1,
    .nameSize = 6,
    .descriptionSize = 11,

    .readChannel = NULL,
    .readChannelType = readType,
    .writeChannelType = NULL,
    .name = (char *)"Device",
    .description = (char *)"Description",
};

uint64_t writeFunc(char *data, uint64_t size, void *obj)
{
    QTcpSocket *socket = (QTcpSocket *)obj;

    if (socket == nullptr)
        return 0;

    auto s = socket->write(data, size);
    socket->flush();
    return s;
}

void slotDataRecived()
{
    buffer += socket->readAll();

    qDebug() << buffer.toHex(':') << '\n';

    if ((uint64_t)buffer.size() < expextedDataSize)
        return;

    while (buffer.size() > 0)
    {
        memcpy(recivedBuffer, buffer.data(), buffer.size()); //!!!

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
                uint64_t size = responseIdentificationData(transmitBuffer, BUFSIZ, &iot);
                socket->write(transmitBuffer, size);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
            {
                struct Read_Write *rwPkg = ((struct Read_Write *)header->pkg);
                if (rwPkg->channelNumber == 0 && rwPkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
                    camera->start();
                else if (rwPkg->channelNumber == 0 && rwPkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
                    camera->stop    ();

                responseReadData(transmitBuffer, BUFSIZ, &iot, header, writeFunc, (void *)socket);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
            {
                uint64_t size = responseWriteData(transmitBuffer, BUFSIZ, &iot, header);
                socket->write(transmitBuffer, size);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_PING_PONG)
            {
                uint64_t size = responsePingData(transmitBuffer, BUFSIZ);
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
    camera->stop();

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

//для ПК
void slotImageCaptured()
{
    QByteArray byteArra;
    QBuffer buffer(&byteArra);
    buffer.open(QIODevice::WriteOnly);
//    QImage img = camera->getImage();
//    img.convertTo(QImage::Format_RGB32);
    camera->getImage().save(&buffer, "JPG", 100);

    iot.readChannel[0].dataSize = byteArra.size();
    iot.readChannel[0].data = byteArra.data();

    struct Read_Write readWrite = {
        .nameSize = iot.nameSize,
        .channelNumber = 0,
        .flags = ReadWrite_FLAGS_OPEN_STREAM,
        .dataSize = iot.readChannel[0].dataSize,
        .name = iot.name,
        .data = iot.readChannel[0].data
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_READ,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite
    };

    responseReadData(transmitBuffer, BUFSIZ, &iot, &header, writeFunc, (void *)socket);
}

void slotInitApp()
{
    camera->stop();

    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 4);

    iot.readChannel[0].dataSize = camera->getImageSavedSize();
    iot.readChannel[1].dataSize = dataSizeonDataType(readType[1]);
    iot.readChannel[2].dataSize = dataSizeonDataType(readType[2]);
    iot.readChannel[3].dataSize = dataSizeonDataType(readType[3]);

    iot.readChannel[0].data = NULL;
    iot.readChannel[1].data = (char *)malloc(iot.readChannel[1].dataSize);
    iot.readChannel[2].data = (char *)malloc(iot.readChannel[2].dataSize);
    iot.readChannel[3].data = (char *)malloc(iot.readChannel[3].dataSize);


    int16_t value = 1280;
    memcpy(iot.readChannel[1].data, &value, iot.readChannel[1].dataSize);
    value = 720;
    memcpy(iot.readChannel[2].data, &value, iot.readChannel[2].dataSize);
    value = 0;
    memcpy(iot.readChannel[3].data, &value, iot.readChannel[3].dataSize);

    server = new QTcpServer;

    QObject::connect(server, &QTcpServer::newConnection, slotNewConnection);
    QObject::connect(camera, &Widget::signalImageCaptured, slotImageCaptured);

    server->listen(QHostAddress("127.0.0.1"), 2028);
    std::cout << "Start service on 127.0.0.1:2028" << std::endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    camera = new Widget;

    QObject::connect(camera, &Widget::signalFirstCapture, slotInitApp);


    return a.exec();
}
