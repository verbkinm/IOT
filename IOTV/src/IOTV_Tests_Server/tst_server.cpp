#include <QtTest>
#include <QCoreApplication>
#include <QTcpSocket>

#include <iostream>

//#include "../lib/Base_Host/base_host.h"
//#include "../IOTV_server/connection_type/tcp_conn_type.h"
//#include "../IOTV_server/connection_type/com_conn_type.h"
//#include "../IOTV_server/connection_type/file_conn_type.h"

//#include "../lib/raw/raw.h"
//#include "../lib/Log/log.h"

//#include "../IOTV_server/IOT_Host/IOT_Host.h"
#include "../IOTV_server/IOT_Server/iot_server.h"

#include "../lib/Protocols/IOTV_SH.h"
#include "../lib/Protocols/iotv_sc.h"


// add necessary includes here

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

    IOT_Host host("test_host");
    qint64 res = IOTV_SH::query_READ(host, 0);

}

QTEST_MAIN(server)

#include "tst_server.moc"
