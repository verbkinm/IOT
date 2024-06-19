#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QBuffer>
#include <QPainter>

#include <QAudioDevice>
#include <QAudioInput>
#include <QUrl>
#include <QThread>

#include <iostream>
#include <fstream>

#include "QtWidgets/qapplication.h"
#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

#include "iotv_types.h"
#include "qudpsocket.h"
#include "widget.h"

Widget *camera;
QTimer *timer_broadcast;

//#define BUFSIZ

QTcpServer *server = nullptr;
QUdpSocket udp_broadcast_send;
QTcpSocket *socket = nullptr;

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = HEADER_SIZE;
uint64_t cutDataSize = 0;
bool error = false;


uint8_t readType[5] = {DATA_TYPE_RAW, DATA_TYPE_RAW, DATA_TYPE_INT_16, DATA_TYPE_INT_16, DATA_TYPE_INT_8};
uint8_t writeType[5] = {DATA_TYPE_NONE, DATA_TYPE_NONE, DATA_TYPE_INT_16, DATA_TYPE_INT_16, DATA_TYPE_INT_8};

QByteArray buffer;

iotv_obj_t iot = {
    .id = 8,
    .numberReadChannel = 5,
    .numberWriteChannel = 5,
    .state = 1,
    .nameSize = 6,
    .descriptionSize = 11,

    .readChannel = NULL,
    .readChannelType = readType,
    .writeChannelType = writeType,
    .name = (char *)"Device",
    .description = (char *)"Description",
};

uint64_t writeFunc(char *data, uint64_t size, void *obj)
{
    QTcpSocket *socket = (QTcpSocket *)obj;

    if (socket == nullptr)
        return 0;

    auto s = socket->write(data, size);
    //    socket->flush();
    return s;
}

void slotDataRecived()
{
    buffer += socket->readAll();

    //    qDebug() << buffer.toHex(':') << '\n';

    if ((uint64_t)buffer.size() < expextedDataSize)
        return;

    while (buffer.size() > 0)
    {
        memcpy(recivedBuffer, buffer.data(), buffer.size()); //!!!

        header_t* header = createPkgs((uint8_t*)recivedBuffer, buffer.size(), &error, &expextedDataSize, &cutDataSize);

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
                uint64_t size = responseIdentificationData(transmitBuffer, BUFSIZ, &iot, Identification_FLAGS_NONE);
                socket->write(transmitBuffer, size);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
            {
                struct Read_Write *rwPkg = ((struct Read_Write *)header->pkg);
                if ((rwPkg->channelNumber == 0 || rwPkg->channelNumber == 1) && rwPkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
                    camera->start();
                else if ((rwPkg->channelNumber == 0 || rwPkg->channelNumber == 1) && rwPkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
                    camera->stop();
                else
                    responseReadData(transmitBuffer, BUFSIZ, &iot, header, writeFunc, (void *)socket, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
            {
                uint64_t size = responseWriteData(transmitBuffer, BUFSIZ, &iot, header, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
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
    
    timer_broadcast->start();
}

//для ПК
void slotNewConnection()
{
    timer_broadcast->stop();
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
    QImage img = camera->getImage();
    img = img.convertToFormat(QImage::Format_RGBA8888);
    //    qDebug() << img.format();

    int width = *(int16_t *)iot.readChannel[2].data;
    int height = *(int16_t *)iot.readChannel[3].data;

    img = img.scaled(width, height, Qt::KeepAspectRatio);
    img.save(&buffer, "JPG", *(int8_t *)iot.readChannel[4].data);

    //    std::ofstream file("image.jpg", std::ios::binary);
    //    file.write(byteArra.data(), byteArra.size());
    //    img.save("image.jpg", "JPG", *(int8_t *)iot.readChannel[4].data);

    iot.readChannel[0].dataSize = byteArra.size();
    iot.readChannel[0].data = byteArra.data();

    struct Read_Write readWrite = {
        .dataSize = iot.readChannel[0].dataSize,
        .nameSize = iot.nameSize,
        .channelNumber = 0,
        .flags = ReadWrite_FLAGS_OPEN_STREAM,
        .name = iot.name,
        .data = iot.readChannel[0].data
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_READ,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite
    };

    qDebug() << 0 << width << height << *(int8_t *)iot.readChannel[4].data << iot.readChannel[0].dataSize;
    responseReadData(transmitBuffer, BUFSIZ, &iot, &header, writeFunc, (void *)socket, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
}

//для ПК
void slotAudio(QByteArray data)
{
    iot.readChannel[1].dataSize = data.size();
    iot.readChannel[1].data = data.data();

    struct Read_Write readWrite = {
        .dataSize = iot.readChannel[1].dataSize,
        .nameSize = iot.nameSize,
        .channelNumber = 1,
        .flags = ReadWrite_FLAGS_OPEN_STREAM,
        .name = iot.name,
        .data = iot.readChannel[1].data
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_READ,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite
    };

    qDebug() << 1 << iot.readChannel[1].dataSize;
    responseReadData(transmitBuffer, BUFSIZ, &iot, &header, writeFunc, (void *)socket, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
}

void slotInitApp()
{
    camera->stop();

    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 5);

    iot.readChannel[0].dataSize = 0;
    iot.readChannel[1].dataSize = 0;
    iot.readChannel[2].dataSize = dataSizeonDataType(readType[2]);
    iot.readChannel[3].dataSize = dataSizeonDataType(readType[3]);
    iot.readChannel[4].dataSize = dataSizeonDataType(readType[4]);

    iot.readChannel[0].data = NULL;
    iot.readChannel[1].data = NULL;
    iot.readChannel[2].data = (char *)malloc(iot.readChannel[2].dataSize);
    iot.readChannel[3].data = (char *)malloc(iot.readChannel[3].dataSize);
    iot.readChannel[4].data = (char *)malloc(iot.readChannel[4].dataSize);


    int16_t value = 640;
    memcpy(iot.readChannel[2].data, &value, iot.readChannel[2].dataSize);
    value = 480;
    memcpy(iot.readChannel[3].data, &value, iot.readChannel[3].dataSize);
    value = 50;
    memcpy(iot.readChannel[4].data, &value, iot.readChannel[4].dataSize);

    server = new QTcpServer;

    QObject::connect(server, &QTcpServer::newConnection, slotNewConnection);
    QObject::connect(camera, &Widget::signalImageCaptured, slotImageCaptured);
    QObject::connect(camera, &Widget::signalAudio, slotAudio);

    server->listen(QHostAddress("127.0.0.1"), 2028);
    std::cout << "Start service on 127.0.0.1:2028" << std::endl;
}

void slotTimeOut()
{
    struct Host_Broadcast hb = {
        .address = QHostAddress("127.0.0.1").toIPv4Address(),
        .port = 2028,
        .nameSize = iot.nameSize,
        .flags = Host_Broadcast_FLAGS_TCP_CONN,
        .name = iot.name
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_HOST_BROADCAST,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = hostBroadCastSize(&hb),
        .pkg = &hb
    };

    char data[BUFSIZ] = {0};
    auto size = headerToData(&header, data, BUFSIZ);
    
    udp_broadcast_send.writeDatagram(data, size, QHostAddress::Broadcast, 2022);

    responseReadData(transmitBuffer, BUFSIZ, &iot, &header, writeFunc, (void *)socket, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    camera = new Widget;

    QObject::connect(camera, &Widget::signalFirstCapture, slotInitApp);
    
    timer_broadcast = new QTimer;
    
    QObject::connect(timer_broadcast, &QTimer::timeout, slotTimeOut);
    
    timer_broadcast->start(1000);

    return a.exec();
}
