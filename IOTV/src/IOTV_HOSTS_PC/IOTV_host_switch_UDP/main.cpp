#include <QCoreApplication>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QNetworkProxy>

#include <iostream>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

QUdpSocket *socket = nullptr;

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = HEADER_SIZE;
uint64_t cutDataSize = 0;
bool error = false;


uint8_t readType[3] = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL};
uint8_t writeType[3] = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL};

QByteArray buffer;

struct IOTV_Server_embedded iot = {
    .id = 1,
    .name = "Device",
    .description = "Description",
    .numberReadChannel = 3,
    .readChannel = NULL,
    .readChannelType = readType,
    .numberWriteChannel = 3,
    .writeChannelType = writeType,
    .state = 1,
    .nameSize = static_cast<uint8_t>(strlen(iot.name)),
    .descriptionSize = static_cast<uint8_t>(strlen(iot.description)),
};


void slotDataRecived()
{
//    char data[1024];
//    QHostAddress peerHost;
//    quint16 peerPort;

//    qint64 len = socket->readDatagram(data, 1024, &peerHost, &peerPort);
//    buffer.append(data, len);


    QNetworkDatagram datagram = socket->receiveDatagram();
//    auto len = _udpSocket.readDatagram(inData, BUFFER_MAX_SIZE);
    buffer.append(datagram.data());


//    buffer += socket->readAll();

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
            QHostAddress peerHost = datagram.senderAddress();
            quint16 peerPort = datagram.senderPort();

            if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
            {
                uint64_t size = responseIdentificationData(transmitBuffer, BUFSIZ, &iot);
                socket->writeDatagram(transmitBuffer, size, peerHost, peerPort);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
            {
                uint64_t size = responseReadData(transmitBuffer, BUFSIZ, &iot, header);
                socket->writeDatagram(transmitBuffer, size, peerHost, peerPort);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
            {
                uint64_t size = responseWriteData(transmitBuffer, BUFSIZ, &iot, header);
                socket->writeDatagram(transmitBuffer, size, peerHost, peerPort);
            }
            else if (header->assignment == HEADER_ASSIGNMENT_PING_PONG)
            {
                uint64_t size = responsePingData(transmitBuffer, BUFSIZ);
                socket->writeDatagram(transmitBuffer, size, peerHost, peerPort);

                QByteArray data;
                data.append(transmitBuffer, size);
                std::cout << peerHost.toString().toStdString().c_str() << ":" << peerPort << " -> " << data.toHex(':').data() << '\n';
            }
            else if (header->assignment == HEADER_ASSIGNMENT_STATE)
            {
                uint64_t size = responseStateData(transmitBuffer, BUFSIZ, &iot);
                socket->writeDatagram(transmitBuffer, size, peerHost, peerPort);
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 3);

//    iot.readChannel[0].dataSize = dataSizeonDataType(readType[0]);
//    iot.readChannel[1].dataSize = dataSizeonDataType(readType[1]);
//    iot.readChannel[2].dataSize = dataSizeonDataType(readType[2]);

//    iot.readChannel[0].data = (char *)malloc(iot.readChannel[0].dataSize);
//    int16_t value = 257;
//    memcpy(iot.readChannel[0].data, &value, iot.readChannel[0].dataSize);

//    iot.readChannel[1].data = (char *)malloc(iot.readChannel[1].dataSize);
//    value = 300;
//    memcpy(iot.readChannel[1].data, &value, iot.readChannel[1].dataSize);

//    iot.readChannel[2].data = (char *)malloc(iot.readChannel[2].dataSize);
//    value = 3;
//    memcpy(iot.readChannel[2].data, &value, iot.readChannel[2].dataSize);


    socket = new QUdpSocket;
    socket->setProxy(QNetworkProxy::NoProxy);
    if (!(socket->bind(QHostAddress::LocalHost, 2025, QAbstractSocket::ShareAddress)))
    {
        std::cerr << "Error bind\n";
        return -1;
    }

//    QObject::connect(socket, &QUdpSocket::readyRead, slotDataRecived);

    std::cout << "Start service on 127.0.0.1:2025" << std::endl;

    return a.exec();
}
