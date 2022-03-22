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
    QByteArray dataComp;
    dataComp.push_back(0x01);

    QCOMPARE(dataComp, IOTV_SH::query_WAY());

    IOT_Host host("test_host", nullptr);
    host.setConnectionTypeTCP("192.168.0.104", 8888);
    host.connectToHost();

    QCOMPARE(dataComp, IOTV_SH::query_READ(0));

    Raw::RAW raw;
    raw.ui64 = 0;
    auto res = IOTV_SH::query_WRITE(host, 0, raw);
    QCOMPARE(11, res); //кол-во байт

    QByteArray expected;
    expected.push_back(0x01);
    QCOMPARE(expected, IOTV_SH::query_WAY());

    QByteArray data;
    IOTV_SH::query_PING(data);
}

QTEST_MAIN(server)

#include "tst_server.moc"
