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
    void test_protocol_IOTV_SH_response();

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
    dataComp.clear();

    //Тест запроса WAY
    {
        dataComp.push_back(0x01);
        QCOMPARE(dataComp, IOTV_SH::query_WAY());
    }

    //Тест запроса READ
    {
        dataComp.clear();
        dataComp.push_back(0x02);
        QCOMPARE(dataComp, IOTV_SH::query_READ(0));

        dataComp.clear();
        dataComp.push_back(0x12);
        QCOMPARE(dataComp, IOTV_SH::query_READ(1));
    }

    //Тест запроса WRITE
    {
        Raw::RAW raw;
        raw.i64 = 0;
        raw.i8 = 1;

        dataComp.clear();
        char masComp[] = {0x10, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        for(int i = 0; i < 11; i++)
            dataComp.push_back(masComp[i]);

        QCOMPARE(dataComp, IOTV_SH::query_WRITE(_host, 1, raw));

        raw.str = new char[11];
        char testStringRaw[] = "test server";
        for (uint8_t i = 0; i < 11; ++i)
            raw.str[i] = testStringRaw[i];

        dataComp.clear();
        dataComp.push_back(0x20);
        dataComp.push_back(char(0x00));
        dataComp.push_back(0x0b);
        dataComp.push_back(testStringRaw);

        QCOMPARE(dataComp, IOTV_SH::query_WRITE(_host, 2, raw));

        delete[] raw.str;
        raw.str = nullptr;
    }

    //Тест запроса PING
    {
        QCOMPARE(QUERY_PING_BYTE, IOTV_SH::query_PING()[0]);
    }
}

void server::test_protocol_IOTV_SH_response()
{
    QByteArray data;

    std::string description = "test description";

    data.push_back(0x05); //way
    data.push_back(0x01); // id
    data.push_back(char(0x00)); // msb size description
    data.push_back(description.size()); //lsb size description
    data.push_back(0x22); // r/w channels count
    data.push_back(description.c_str());//description text

    //Типы каналов чтения
    data.push_back(Raw::toUInt8(Raw::DATA_TYPE::INTEGER_8));
    data.push_back(Raw::toUInt8(Raw::DATA_TYPE::CHAR_PTR));

    //Типы каналов записи
    data.push_back(Raw::toUInt8(Raw::DATA_TYPE::INTEGER_8));
    data.push_back(Raw::toUInt8(Raw::DATA_TYPE::CHAR_PTR));

    //Тест ответа WAY
    {
        IOTV_SH::response_WAY(_host, data);
        QCOMPARE(1, _host.getId());
        QCOMPARE(description.c_str(), _host.getDescription());

        QCOMPARE(2, _host.readChannelLength());
        QCOMPARE(2, _host.writeChannelLength());

        QCOMPARE(Raw::DATA_TYPE::INTEGER_8, _host.getReadChannelDataType(0));
        QCOMPARE(Raw::DATA_TYPE::CHAR_PTR, _host.getReadChannelDataType(1));

        QCOMPARE(Raw::DATA_TYPE::INTEGER_8, _host.getWriteChannelDataType(0));
        QCOMPARE(Raw::DATA_TYPE::CHAR_PTR, _host.getWriteChannelDataType(1));

        //мусорные значения

    }

    //Тест ответа READ
    {
        {
            data.clear();
            data.push_back(0x06); //0-й канал, ответ READ
            data.push_back(char(0x00));
            data.push_back(0x08);

            Raw::RAW raw;
            raw.i64 = 1;

            for (uint8_t i = 0; i < Raw::size; ++i)
                data.push_back(raw.array[i]);

            IOTV_SH::response_READ(_host, data);

            QCOMPARE(raw, _host.getReadChannelData(0));
        }
        {
            std::string text = "test text";
            data.clear();
            data.push_back(0x16); //1-й канал, ответ READ
            data.push_back(char(0x00));
            data.push_back(text.size());
            data.push_back(text.c_str());

            IOTV_SH::response_READ(_host, data);
            QCOMPARE(text.c_str(), _host.getReadChannelData(1).str);
            QCOMPARE(nullptr, _host.getReadChannelData(2).str); // канала 2 нет, _host.getReadChannelData(2) возвартит нулевой RAW
        }
    }

    //Тест ответа WRITE
    {
        data.clear();
        IOTV_SH::response_WRITE(_host, data); //response_WRITE ничего не делает, остался для дальнейшей доработки
        QCOMPARE(0, data.size());
    }

    //Тест ответа PONG
    {
        IOTV_SH::response_PONG(data);
        QCOMPARE(RESPONSE_PONG_BYTE, data[0]);
    }
}

QTEST_MAIN(server)

#include "tst_server.moc"
