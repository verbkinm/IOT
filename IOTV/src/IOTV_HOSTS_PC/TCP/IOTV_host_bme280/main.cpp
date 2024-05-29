#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include <iostream>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"
#include "iotv_types.h"

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;

QTimer *timer = nullptr;

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = HEADER_SIZE;
uint64_t cutDataSize = 0;
bool error = false;


uint8_t readType[3] = {DATA_TYPE_FLOAT_32, DATA_TYPE_FLOAT_32, DATA_TYPE_FLOAT_32};
//uint8_t writeType[3] = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL};

QByteArray buffer;

iotv_obj_t iot = {
    .id = 2,
    .numberReadChannel = 3,
    .numberWriteChannel = 0,
    .state = 1,
    .nameSize = 7,
    .descriptionSize = 51,

    .readChannel = NULL,
    .readChannelType = readType,
    .writeChannelType = NULL,

    .name = (char *)"bme-280",
    .description = (char *)"Главней всего, погода в доме",
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

void slotTimerOut()
{
    float t = (rand() % (2500 - (1800) + 1) + (1800)) / 100.0;
    memcpy(iot.readChannel[0].data, &t, iot.readChannel[0].dataSize);

    float h = (rand() % (10000 - 7000 + 1) + 7000) / 100.0;
    memcpy(iot.readChannel[1].data, &h, iot.readChannel[1].dataSize);

    float p = (rand() % (76000 - (73000) + 1) + (73000)) / 100.0;
    memcpy(iot.readChannel[2].data, &p, iot.readChannel[2].dataSize);

//    qDebug() << t << h << p;
}

int main(int argc, char *argv[])
{
    srand(time(0));

    QCoreApplication a(argc, argv);

    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 3);

    iot.readChannel[0].dataSize = dataSizeonDataType(readType[0]);
    iot.readChannel[1].dataSize = dataSizeonDataType(readType[1]);
    iot.readChannel[2].dataSize = dataSizeonDataType(readType[2]);

    iot.readChannel[0].data = (char *)malloc(iot.readChannel[0].dataSize);
    float value = -10.7;
    memcpy(iot.readChannel[0].data, &value, iot.readChannel[0].dataSize);

    iot.readChannel[1].data = (char *)malloc(iot.readChannel[1].dataSize);
    value = 78.69369;
    memcpy(iot.readChannel[1].data, &value, iot.readChannel[1].dataSize);

    iot.readChannel[2].data = (char *)malloc(iot.readChannel[2].dataSize);
    value = 570.639;
    memcpy(iot.readChannel[2].data, &value, iot.readChannel[2].dataSize);

    server = new QTcpServer;

    QObject::connect(server, &QTcpServer::newConnection, [&](){
        slotNewConnection();
    });

    server->listen(QHostAddress("127.0.0.1"), 2025);
    std::cout << "Start service on 127.0.0.1:2025" << std::endl;

    timer = new QTimer;

    QObject::connect(timer, &QTimer::timeout, slotTimerOut);

    timer->start(500);

    return a.exec();
}
