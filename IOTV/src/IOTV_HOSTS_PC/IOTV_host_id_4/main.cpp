#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include <iostream>

#include "Protocols/IOTV_SH.h"
#include "iot_server.h"

QTcpServer *server = nullptr;
QTcpSocket *socket = nullptr;

IOTV_Server iot;

char recivedBuffer[BUFSIZ] {0}, transmitBuffer[BUFSIZ] {0};
char* ptrBuf = recivedBuffer;

void debug()
{
    qDebug() << "Play: " << iot.play()
             << ", Led: " << iot.led()
             << ", Repeate: " << iot.repeate()
             << ", Mode: " << iot.mode();
}

void slotDataRecived()
{
    uint16_t dataSize = 0;
    uint64_t recvSize = socket->read(ptrBuf, BUFSIZ); //!!!
    ptrBuf += recvSize;

//    if (ptrBuf > recivedBuffer + BUFSIZ - 1)
//        ptrBuf = recivedBuffer;

    while(ptrBuf != recivedBuffer)
    {
        if (recivedBuffer[0] == Protocol_class::QUERY_WAY_BYTE)
        {
            dataSize = Protocol_class::response_WAY(iot, transmitBuffer);
            socket->write(transmitBuffer, dataSize);
            memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFSIZ - 1);
            ptrBuf--;
        }
        else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_READ_BYTE)
        {
            dataSize = Protocol_class::response_READ(iot, recivedBuffer, ptrBuf, transmitBuffer);

            socket->write(transmitBuffer, dataSize);
            memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFSIZ - 1);
            ptrBuf--;
        }
        else if (recivedBuffer[0] == Protocol_class::QUERY_PING_BYTE)
        {
            dataSize = Protocol_class::response_Pong(transmitBuffer);
            socket->write(transmitBuffer, dataSize);
            memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFSIZ - 1);
            ptrBuf--;
        }
        else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_WRITE_BYTE)
        {
            //локальный dataSize
            int dataSize = Protocol_class::response_WRITE(iot, recivedBuffer, ptrBuf, transmitBuffer);
            if (dataSize >= 0)
            {
                socket->write(transmitBuffer, 1); // ответ на write = 1 байт
                memmove((void*)recivedBuffer, (void*)&recivedBuffer[dataSize + 3], BUFSIZ - (dataSize + 3));
                ptrBuf -= dataSize + 3;
            }
        }
        else
            ptrBuf = recivedBuffer;

        if (ptrBuf < recivedBuffer)
            ptrBuf = recivedBuffer;

        debug();
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
    QTimer timer;
    timer.setInterval(5000);
    timer.start();

    server = new QTcpServer;

    QObject::connect(server, &QTcpServer::newConnection, [&](){
        slotNewConnection();
    });

//    QObject::connect(&timer, &QTimer::timeout, [&](){
//       iot.setPlay(!iot.play());
//    });

    server->listen(QHostAddress("127.0.0.1"), 2025);
    std::cout << "Start service on 127.0.0.1:2025" << std::endl;

    return a.exec();
}
