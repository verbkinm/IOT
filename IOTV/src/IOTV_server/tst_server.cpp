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
    void test_protocol_IOTV_SH_query();

private:
    IOT_Host _host;
    //    IOT_Server _iot_server;
};

server::server() : _host("test_host", nullptr)
{
    _host.setId(1);
    _host.setDescription("test_host");
    _host.addReadSubChannel(Raw::DATA_TYPE::INTEGER_8);
    _host.addReadSubChannel(Raw::DATA_TYPE::BOOL_8);
    _host.addReadSubChannel(Raw::DATA_TYPE::CHAR_PTR);

    _host.addWriteSubChannel(Raw::DATA_TYPE::INTEGER_8);
    _host.addWriteSubChannel(Raw::DATA_TYPE::BOOL_8);

    _host.setConnectionTypeTCP("192.168.0.104", 8888);
    _host.connectToHost();
}

server::~server()
{
    _host.disconnect();
}

void server::initTestCase()
{
    //    std::cout << std::boolalpha << _iot_server.isListening() << '\n';
}

void server::cleanupTestCase()
{

}

void server::test_protocol_IOTV_SH_query()
{
    QByteArray dataComp;

    dataComp.push_back(0x01);
    QCOMPARE(dataComp, IOTV_SH::query_WAY());

    dataComp.clear();
    dataComp.push_back(0x02);
    QCOMPARE(dataComp, IOTV_SH::query_READ(0));

    dataComp.clear();
    dataComp.push_back(0x12);
    QCOMPARE(dataComp, IOTV_SH::query_READ(1));

    Raw::RAW raw;
    raw.ui64 = 0;
    raw.i8 = 1;
    dataComp.clear();
    char masComp[] = {0x10, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for(int i = 0; i < 11; i++)
        dataComp.push_back(masComp[i]);
    QCOMPARE(dataComp, IOTV_SH::query_WRITE(_host, 1, raw));

    char *ptr = new char[11];
    char testStringRaw[] = "test server";
    for (uint8_t i = 0; i < 11; ++i)
        ptr[i] = testStringRaw[i];

    raw.str = ptr;
    dataComp.clear();
    dataComp.push_back(0x20);
    dataComp.push_back(char(0x00));
    dataComp.push_back(0x0b);
    dataComp.push_back(testStringRaw);
    QCOMPARE(dataComp, IOTV_SH::query_WRITE(_host, 2, raw));
    delete[] raw.str;
    raw.str = nullptr;

    dataComp.clear();
    dataComp.push_back(0x08);
    QCOMPARE(dataComp, IOTV_SH::query_PING());
}

QTEST_MAIN(server)

#include "tst_server.moc"
