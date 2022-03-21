#include <QtTest>
#include <QCoreApplication>

#include <iostream>

#include "IOT_Server/iot_server.h"
#include "wrapper.h"

class server : public QObject
{
    Q_OBJECT

public:
    server();
    ~server();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_protocol_IOTV_SH();

private:
//    IOT_Server _iot_server;
};

server::server()
{
    std::cout << "server" << '\n';

}

server::~server()
{
    std::cout << "~server" << '\n';
}

void server::initTestCase()
{
//    std::cout << std::boolalpha << _iot_server.isListening() << '\n';
}

void server::cleanupTestCase()
{

}

void server::test_protocol_IOTV_SH()
{
    QByteArray data;
    IOTV_SH::query_WAY(data);

    QByteArray dataComp;
    dataComp.push_back(0x01);

    QCOMPARE(data, dataComp);
    QCOMPARE(data, dataComp); //не пустой data

    IOT_Host host("test_host", nullptr);
    host.setConnectionTypeTCP("192.168.0.104", 8888);
    host.connectToHost();
    qint64 res = IOTV_SH::query_READ(host, 0);
    QCOMPARE(1, res); //кол-во байт

    Raw::RAW raw;
    raw.ui64 = 0;
    res = IOTV_SH::query_WRITE(host, 0, raw);
    QCOMPARE(11, res); //кол-во байт
}

QTEST_MAIN(server)

#include "tst_server.moc"
