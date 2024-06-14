#include <QCoreApplication>
#include <QTcpSocket>

#include <iostream>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"
#include "iotv_types.h"

QTcpSocket *socket = nullptr;

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = HEADER_SIZE;
uint64_t cutDataSize = 0;
bool error = false;


uint8_t readType[3] = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL};
uint8_t writeType[3] = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL};

QByteArray buffer;

iotv_obj_t iot = {
    .id = 1,
    .numberReadChannel = 3,
    .numberWriteChannel = 3,
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
        memcpy(recivedBuffer, buffer.data(), buffer.size());

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

    exit(1);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 3);

    iot.readChannel[0].dataSize = dataSizeonDataType(readType[0]);
    iot.readChannel[1].dataSize = dataSizeonDataType(readType[1]);
    iot.readChannel[2].dataSize = dataSizeonDataType(readType[2]);

    iot.readChannel[0].data = (char *)malloc(iot.readChannel[0].dataSize);
    int16_t value = 257;
    memcpy(iot.readChannel[0].data, &value, iot.readChannel[0].dataSize);

    iot.readChannel[1].data = (char *)malloc(iot.readChannel[1].dataSize);
    value = 300;
    memcpy(iot.readChannel[1].data, &value, iot.readChannel[1].dataSize);

    iot.readChannel[2].data = (char *)malloc(iot.readChannel[2].dataSize);
    value = 3;
    memcpy(iot.readChannel[2].data, &value, iot.readChannel[2].dataSize);

    socket = new QTcpSocket;
    socket->connectToHost(QHostAddress("127.0.0.1"), 2023);

    QObject::connect(socket, &QTcpSocket::readyRead, slotDataRecived);
    QObject::connect(socket, &QTcpSocket::disconnected, slotDisconnected);
    QObject::connect(socket, &QTcpSocket::disconnected, &QObject::deleteLater);

    return a.exec();
}
