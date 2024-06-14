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
#include "qnetworkdatagram.h"
#include "qudpsocket.h"
#include "widget.h"

Widget *camera;
QTimer *timer_broadcast;

QUdpSocket *udp_broadcast_send, *udp_socket;

QHostAddress serverAddr;
quint16 serverPort;

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
    QNetworkDatagram *datagram = (QNetworkDatagram *)obj;

    if (datagram == nullptr)
        return 0;

    QHostAddress peerHost = datagram->senderAddress();
    quint16 peerPort = datagram->senderPort();

    auto s = udp_socket->writeDatagram(data, size, peerHost, peerPort);
    return s;
}

void slotDataRecived()
{
    QNetworkDatagram datagram = udp_socket->receiveDatagram();
    buffer.append(datagram.data());

    qDebug() << buffer.toHex(':') << '\n';

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
            timer_broadcast->start();

            serverAddr = datagram.senderAddress();
            serverPort = datagram.senderPort();

            if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
            {
                uint64_t size = responseIdentificationData(transmitBuffer, BUFSIZ, &iot, Identification_FLAGS_NONE);
                udp_socket->writeDatagram(transmitBuffer, size, serverAddr, serverPort);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
            {
                struct Read_Write *rwPkg = ((struct Read_Write *)header->pkg);
                if ((rwPkg->channelNumber == 0 || rwPkg->channelNumber == 1) && rwPkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
                    camera->start();
                else if ((rwPkg->channelNumber == 0 || rwPkg->channelNumber == 1) && rwPkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
                    camera->stop();

                responseReadData(transmitBuffer, BUFSIZ, &iot, header, writeFunc, (void *)&datagram, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
            {
                uint64_t size = responseWriteData(transmitBuffer, BUFSIZ, &iot, header, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
                udp_socket->writeDatagram(transmitBuffer, size, serverAddr, serverPort);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_PING_PONG)
            {
                uint64_t size = responsePingData(transmitBuffer, BUFSIZ);
                udp_socket->writeDatagram(transmitBuffer, size, serverAddr, serverPort);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_STATE)
            {
                uint64_t size = responseStateData(transmitBuffer, BUFSIZ, &iot);
                udp_socket->writeDatagram(transmitBuffer, size, serverAddr, serverPort);
            }
        }
        buffer = buffer.mid(cutDataSize);
    }
}

//для ПК
void slotImageCaptured(QImage img)
{
    QByteArray byteArra;
    QBuffer buffer(&byteArra);
    buffer.open(QIODevice::WriteOnly);
//    QImage img = camera->getImage();
    //    img = img.convertToFormat(QImage::Format_RGBA8888);
    //    qDebug() << img.format();

    int width = *(int16_t *)iot.readChannel[2].data;
    int height = *(int16_t *)iot.readChannel[3].data;

    img = img.scaled(width, height, Qt::KeepAspectRatio);
    img.save(&buffer, "JPG", *(int8_t *)iot.readChannel[4].data);

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

    QNetworkDatagram dataGram;
    dataGram.setSender(serverAddr, serverPort);
    responseReadData(transmitBuffer, BUFSIZ, &iot, &header, writeFunc, (void *)&dataGram, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
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

//    responseReadData(transmitBuffer, BUFSIZ, &iot, &header, writeFunc, (void *)socket);
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

    udp_socket = new QUdpSocket;
    if (!(udp_socket->bind(QHostAddress::AnyIPv4, 2028, QAbstractSocket::ShareAddress)))
    {
        std::cerr << "Error bind\n";
        exit(1);
    }

    std::cout << "Start service on 127.0.0.1:2028" << std::endl;

    QObject::connect(udp_socket, &QUdpSocket::readyRead, slotDataRecived);
    QObject::connect(camera, &Widget::signalImageCaptured, slotImageCaptured);
    //    QObject::connect(camera, &Widget::signalAudio, slotAudio);
}

void slotTimeOut()
{
    struct Host_Broadcast hb = {
        .address = QHostAddress("127.0.0.1").toIPv4Address(),
        .port = 2028,
        .nameSize = iot.nameSize,
        .flags = Host_Broadcast_FLAGS_UDP_CONN,
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

    udp_broadcast_send->writeDatagram(data, size, QHostAddress::Broadcast, 2022);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    udp_broadcast_send = new QUdpSocket;
    camera = new Widget;

    QObject::connect(camera, &Widget::signalFirstCapture, slotInitApp);

    timer_broadcast = new QTimer;
    QObject::connect(timer_broadcast, &QTimer::timeout, slotTimeOut);
    timer_broadcast->start(5000);

    return a.exec();
}
