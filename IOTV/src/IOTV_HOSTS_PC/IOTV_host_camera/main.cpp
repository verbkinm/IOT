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
char imageBuffer[1024 * 1024 * 5];

#define BUFSIZE 1024

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;
QTimer *timer = nullptr;

char recivedBuffer[BUFSIZE], transmitBuffer[BUFSIZE];
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
                uint64_t size = responseIdentificationData(transmitBuffer, BUFSIZE, &iot);
                socket->write(transmitBuffer, size);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
            {
                struct Read_Write *ptrReadWrite = ((struct Read_Write *)header->pkg);
                if (ptrReadWrite->flags == ReadWrite_FLAGS_OPEN_STREAM)
                {
                    auto pkgResponceCount = responceReadWritePkgCount(BUFSIZE, &iot, header);
                    if (pkgResponceCount > 1)
                    {
                        QByteArray imgArray;
                        QBuffer buffer(&imgArray);
                        buffer.open(QIODevice::ReadWrite);
                        camera->getImage().save(&buffer, "JPG");
                        buffer.seek(0);

                        for (uint64_t i = 0; i < pkgResponceCount; ++i)
                        {
                            auto dataRead = buffer.read(BUFSIZ - (HEADER_SIZE + READ_WRITE_SIZE + strlen(iot.name)));

                            struct Read_Write readWrite = {
                                .nameSize = static_cast<uint8_t>(strlen(iot.name)),
                                .channelNumber = ptrReadWrite->channelNumber,
                                .flags = ReadWrite_FLAGS_NONE,
                                .dataSize = static_cast<uint64_t>(dataRead.size()),
                                .name = iot.name,
                                .data = dataRead.data()
                            };

                            struct Header header = {
                                .version = 2,
                                .type = HEADER_TYPE_RESPONSE,
                                .assignment = HEADER_ASSIGNMENT_READ,
                                .flags = HEADER_FLAGS_NONE,
                                .fragment = static_cast<uint16_t>(i + 1),
                                .fragments = static_cast<uint16_t>(pkgResponceCount),
                                .dataSize = readWriteSize(&readWrite),
                                .pkg = &readWrite
                            };

                            auto size = headerToData(&header, transmitBuffer, BUFSIZE);
                            socket->write(transmitBuffer, size);
                        }
                    }
                }
                else
                {
                    uint64_t size = responseReadData(transmitBuffer, BUFSIZE, &iot, header);
                    socket->write(transmitBuffer, size);
                }

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

//для ПК
void slotTimerOut()
{
    //    auto image = camera->getImage();

    //    image.save("Image.jpg");
    //    size_t imageBytes = image.sizeInBytes();
    //    size_t imageBytesLeft = imageBytes;

    //    uint8_t outData[BUFSIZE];

    //    auto rawDataImg = image.bits();

    //    std::ofstream file;
    //    file.open("Image2.jpg", std::ios_base::binary);
    //    file.write((char *) rawDataImg, imageBytes);

    //    qDebug() <<imageBytes;


    //    while (imageBytesLeft > 0)
    //    {
    //        size_t dataSize = imageBytesLeft;
    //        if (dataSize > (size_t)(BUFSIZE - (HEADER_SIZE + READ_WRITE_SIZE + iot.nameSize)))
    //            dataSize = (BUFSIZE - (HEADER_SIZE + READ_WRITE_SIZE + iot.nameSize));

    //        struct Read_Write readWrite = {
    //            .nameSize = iot.nameSize,
    //            .channelNumber = 0,
    //            .flags = ReadWrite_FLAGS_OPEN_STREAM,
    //            .dataSize = dataSize,
    //            .name = iot.name,
    //            .data = (char *)rawDataImg + (imageBytes - imageBytesLeft)
    //        };

    //        struct Header header = {
    //            .version = 2,
    //            .type = HEADER_TYPE_RESPONSE,
    //            .assignment = HEADER_ASSIGNMENT_READ,
    //            .flags = HEADER_FLAGS_NONE,
    //            .dataSize = readWriteSize(&readWrite),
    //            .pkg = &readWrite,
    //        };

    //        auto size = headerToData(&header, (char *)&outData, BUFSIZE);

    //        //        file.write(readWrite.data, readWrite.dataSize);
    //        socket->write((char *)&outData, size);

    //        imageBytesLeft -= dataSize;
    //    }

    //    timer->stop();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    camera = new Widget;
    //    timer = new QTimer;
    //    timer->stop();
    //    timer->setInterval(100);

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

    QObject::connect(server, &QTcpServer::newConnection, [&](){
        slotNewConnection();
    });

    QObject::connect(timer, &QTimer::timeout, slotTimerOut);

    server->listen(QHostAddress("127.0.0.1"), 2028);
    std::cout << "Start service on 127.0.0.1:2028" << std::endl;

    return a.exec();
}
