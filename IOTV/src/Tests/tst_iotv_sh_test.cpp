#include <QtTest>

#include <cstring>

#include "IOTV_SH.h"

// add necessary includes here

class IOTV_SH_test : public QObject
{
    Q_OBJECT

public:
    IOTV_SH_test();
    ~IOTV_SH_test();

private slots:
    void query();
    void response();

};

IOTV_SH_test::IOTV_SH_test()
{

}

IOTV_SH_test::~IOTV_SH_test()
{

}

void IOTV_SH_test::query()
{    
    {
        //Запрос WAY
        QCOMPARE(IOTV_SH::query_WAY().length(), 1);
        QCOMPARE(IOTV_SH::query_WAY().at(0), IOTV_SH::QUERY_WAY_BYTE);
    }

    {
        //Запрос READ
        uint8_t channelNumber = 1;

        QCOMPARE(IOTV_SH::query_READ(0x00).length(), 1);
        QCOMPARE(IOTV_SH::query_READ(channelNumber).at(0), (channelNumber << 4) | IOTV_SH::QUERY_READ_BYTE);
    }

    {
        //Запрос WRITE
        uint8_t channelNumber = 1;

        Raw raw(Raw::DATA_TYPE::BOOL);
        raw.push_back(1);

        QByteArray data;
        data.push_back((channelNumber << 4) | IOTV_SH::QUERY_WRITE_BYTE);
        data.push_back(raw.size() >> 8);
        data.push_back(raw.size());
        data.push_back(raw.data().at(0));

//        QCOMPARE(IOTV_SH::query_WRITE(channelNumber, raw).length(), 4);
//        QCOMPARE(IOTV_SH::query_WRITE(channelNumber, raw), data);
    }

    {
        //Запрос PING
        QCOMPARE(IOTV_SH::query_PING().length(), 1);
        QCOMPARE(IOTV_SH::query_PING().at(0), IOTV_SH::QUERY_PING_BYTE);
    }
}

void IOTV_SH_test::response()
{
    {
        //Ответ WAY
        IOTV_SH::RESPONSE_WAY pkg;
        pkg.id = 3;
        pkg.description = "test";
        pkg.readChannel = {Raw::DATA_TYPE::BOOL};
        pkg.writeChannel = {Raw::DATA_TYPE::BOOL};

        const char arr[] = {static_cast<char>(IOTV_SH::RESPONSE_WAY_BYTE),
                            static_cast<char>(pkg.id),
                            static_cast<char>(pkg.description.size() >> 8),
                            static_cast<char>(pkg.description.size()),
                            static_cast<char>((pkg.readChannel.size() << 4) | pkg.writeChannel.size())
                           };

        QByteArray data(QByteArray::fromRawData(arr, 5));

//        for (auto ch : pkg.description)
//            data.push_back(ch);

        data.push_back(static_cast<char>(pkg.readChannel.at(0)));
        data.push_back(static_cast<char>(pkg.writeChannel.at(0)));

        int onePkg = data.size();

        data = data.repeated(3);
        data.push_back('\0'); // мусорный байт

        QCOMPARE(data.size(), onePkg * 3 + 1);

        QCOMPARE(IOTV_SH::accumPacket(data)->type, pkg.type);
        QCOMPARE(data.size(), onePkg * 2 + 1);

        QCOMPARE(IOTV_SH::accumPacket(data)->type, pkg.type);
        QCOMPARE(data.size(), onePkg + 1);

        QCOMPARE(IOTV_SH::accumPacket(data)->type, pkg.type);
        QCOMPARE(data.size(), 1);

        QCOMPARE(IOTV_SH::accumPacket(data), nullptr);

        data = QByteArray::fromRawData(arr, 5);
        QCOMPARE(IOTV_SH::accumPacket(data)->type, IOTV_SH::Response_Type::RESPONSE_INCOMPLETE);
    }

    {
        //Ответ READ
        uint8_t channelNumber = 1;
        uint16_t digit = 257;

        QByteArray data;
        data.push_back(digit >> 8);
        data.push_back(digit);

        IOTV_SH::RESPONSE_READ pkg;
        pkg.chanelNumber = channelNumber;
        pkg.data = data;

        data.clear();
        data.push_back((channelNumber << 4) | IOTV_SH::RESPONSE_READ_BYTE);
        data.push_back(pkg.data.size() >> 8);
        data.push_back(pkg.data.size());
        data.append(pkg.data);

        int onePkg = data.size();
        data = data.repeated(3);
        data.push_back('\0'); // мусорный байт

        QCOMPARE(data.size(), onePkg * 3 + 1);

        QCOMPARE(IOTV_SH::accumPacket(data)->type, pkg.type);
        QCOMPARE(data.size(), onePkg * 2 + 1);

        QCOMPARE(IOTV_SH::accumPacket(data)->type, pkg.type);
        QCOMPARE(data.size(), onePkg + 1);

        QCOMPARE(IOTV_SH::accumPacket(data)->type, pkg.type);
        QCOMPARE(data.size(), 1);

        uint16_t result;
        std::memcpy(&result, pkg.data.data(), sizeof(result));

        QCOMPARE(result, digit);

        QCOMPARE(IOTV_SH::accumPacket(data), nullptr);

        data.clear();
        data.push_back((channelNumber << 4) | IOTV_SH::RESPONSE_READ_BYTE);
        QCOMPARE(IOTV_SH::accumPacket(data)->type, IOTV_SH::Response_Type::RESPONSE_INCOMPLETE);
    }

    {
        //Ответ WRITE
        uint8_t channelNumber = 1;

        IOTV_SH::RESPONSE_WRITE pkg;
        pkg.chanelNumber = channelNumber;

        QByteArray data;
        data.push_back((channelNumber << 4) | IOTV_SH::RESPONSE_WRITE_BYTE);

        QCOMPARE(IOTV_SH::accumPacket(data)->type, pkg.type);
    }

    {
        //Ответ PONG
        IOTV_SH::RESPONSE_PONG pkg;
        pkg.state = true;

        QByteArray data;
        data.push_back(IOTV_SH::RESPONSE_PONG_BYTE);

        QCOMPARE(IOTV_SH::accumPacket(data)->type, pkg.type);
    }

    {
        //ОШИБКИ
        IOTV_SH::RESPONSE_PONG pkg;
        pkg.state = true;

        QByteArray data;
        data.push_back(0x10 | IOTV_SH::RESPONSE_PONG_BYTE);


        QVERIFY(IOTV_SH::accumPacket(data) == nullptr);
    }
}

QTEST_APPLESS_MAIN(IOTV_SH_test)

#include "tst_iotv_sh_test.moc"
