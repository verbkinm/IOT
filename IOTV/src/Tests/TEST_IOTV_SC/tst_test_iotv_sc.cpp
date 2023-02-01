#include <QtTest>

#include "IOTV_SC/IOTV_SC.h"

class TEST_IOTV_SC : public QObject
{
    Q_OBJECT

public:
    TEST_IOTV_SC();
    ~TEST_IOTV_SC();

private slots:
    void Client_TX_tset_longName();

    void Client_TX__queryDevice_List();
    void Client_TX__query_STATE();
    void Client_TX__query_READ();
    void Client_TX__query_WRITE();


    void Client_RX__createResponse_DEV_LIST_PKG();
    void Client_RX__createResponse_STATE_PKG();
    void Client_RX__createResponse_READ_PKG();
    void Client_RX__createResponse_WRITE_PKG();
};

TEST_IOTV_SC::TEST_IOTV_SC()
{

}

TEST_IOTV_SC::~TEST_IOTV_SC()
{

}

void TEST_IOTV_SC::Client_TX_tset_longName()
{
    QString devName = "Test dev name looooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong";

    QByteArray data;
    uint8_t nameLength = devName.size() << 3;
    data.push_back( nameLength | IOTV_SC::QUERY_STATE_FIRST_BYTE);
    data.push_back(IOTV_SC::QUERY_STATE_SECOND_BYTE);
    data.append(devName.mid(0, (nameLength >> 3)).toLocal8Bit());

    QCOMPARE(Client_TX::query_STATE(devName), data);
}

void TEST_IOTV_SC::Client_TX__queryDevice_List()
{
    QCOMPARE(Client_TX::query_Device_List().size(), 1);
    QCOMPARE(Client_TX::query_Device_List()[0], 1);
}

void TEST_IOTV_SC::Client_TX__query_STATE()
{
    QString devName = "Test dev name 123";

    QByteArray data;
    data.push_back( (devName.size() << 3) | IOTV_SC::QUERY_STATE_FIRST_BYTE);
    data.push_back(IOTV_SC::QUERY_STATE_SECOND_BYTE);
    data.append(devName.toLocal8Bit());

    QCOMPARE(Client_TX::query_STATE(devName), data);
}

void TEST_IOTV_SC::Client_TX__query_READ()
{
    QString devName = "Test dev name 123";
    uint8_t channelNUmber = 1;

    QByteArray data;
    data.push_back((devName.size() << 3) | IOTV_SC::QUERY_READ_BYTE);
    data.push_back(channelNUmber);
    data.append(devName.toLocal8Bit());

    QCOMPARE(Client_TX::query_READ(devName, channelNUmber), data);
}

void TEST_IOTV_SC::Client_TX__query_WRITE()
{
    QString devName = "Test dev name 123";
    uint8_t channelNUmber = 1;

    QByteArray data, rawData;
    rawData.push_back(1);
    rawData.push_back(2);
    rawData.push_back(3);
    rawData.push_back(4);
    rawData.push_back(5);

    data.push_back((devName.size() << 3) | IOTV_SC::QUERY_WRITE_BYTE);
    data.push_back(channelNUmber);
    data.push_back(rawData.size() >> 8);
    data.push_back(rawData.size());
    data.append(devName.toLocal8Bit());
    data.append(rawData);

    QCOMPARE(Client_TX::query_WRITE(devName, channelNUmber, rawData), data);
}

void TEST_IOTV_SC::Client_RX__createResponse_DEV_LIST_PKG()
{
    Client_RX::RESPONSE_DEV_LIST_PKG pkg;

    Client_RX::DEV_PKG devPkg1;
    devPkg1.id = 1;
    devPkg1.error = false;
    devPkg1.name = "First dev";
    devPkg1.description = "First dev description";
    devPkg1.readChannel.push_back(Raw::DATA_TYPE::BOOL);
    devPkg1.readChannel.push_back(Raw::DATA_TYPE::DOUBLE_64);
    devPkg1.writeChannel.push_back(Raw::DATA_TYPE::BOOL);
    devPkg1.writeChannel.push_back(Raw::DATA_TYPE::DOUBLE_64);

    Client_RX::DEV_PKG devPkg2;
    devPkg2.id = 2;
    devPkg2.error = false;
    devPkg2.name = "Second dev";
    devPkg2.description = "Second dev description";
    devPkg2.readChannel.push_back(Raw::DATA_TYPE::BOOL);
    devPkg2.readChannel.push_back(Raw::DATA_TYPE::DOUBLE_64);
    devPkg2.writeChannel.push_back(Raw::DATA_TYPE::BOOL);
    devPkg2.writeChannel.push_back(Raw::DATA_TYPE::DOUBLE_64);

    pkg.devs.push_back(devPkg1);
    pkg.devs.push_back(devPkg2);

    QByteArray data;
    data.push_back(IOTV_SC::RESPONSE_DEV_LIST_BYTE);
    data.push_back(pkg.devs.size());

    data.push_back(devPkg1.name.size() << 3);
    data.append(devPkg1.name.toLocal8Bit());
    data.push_back(devPkg1.id);
    data.push_back(devPkg1.description.size() >> 8);
    data.push_back(devPkg1.description.size());
    data.push_back((devPkg1.readChannel.size() << 4) | devPkg1.writeChannel.size());
    data.append(devPkg1.description.toLocal8Bit());

    for (uint8_t i = 0; i < devPkg1.readChannel.size(); i++)
        data.push_back(static_cast<uint8_t>(devPkg1.readChannel.at(i)));

    for (uint8_t i = 0; i < devPkg1.writeChannel.size(); i++)
        data.push_back(static_cast<uint8_t>(devPkg1.writeChannel.at(i)));

    data.push_back(devPkg2.name.size() << 3);
    data.append(devPkg2.name.toLocal8Bit());
    data.push_back(devPkg2.id);
    data.push_back(devPkg2.description.size() >> 8);
    data.push_back(devPkg2.description.size());
    data.push_back((devPkg2.readChannel.size() << 4) | devPkg2.writeChannel.size());
    data.append(devPkg2.description.toLocal8Bit());

    for (uint8_t i = 0; i < devPkg2.readChannel.size(); i++)
        data.push_back(static_cast<uint8_t>(devPkg2.readChannel.at(i)));

    for (uint8_t i = 0; i < devPkg2.writeChannel.size(); i++)
        data.push_back(static_cast<uint8_t>(devPkg2.writeChannel.at(i)));


    QByteArray copyData = data;

    {
        QVERIFY(*static_cast<Client_RX::RESPONSE_DEV_LIST_PKG*>(Client_RX::accumPacket(data)) == pkg);
        QCOMPARE(data.size(), 0);

        data.push_back(0xFF);

        QVERIFY(static_cast<Client_RX::RESPONSE_DEV_LIST_PKG*>(Client_RX::accumPacket(data))->type == Client_RX::Response_Type::RESPONSE_ERROR);
        QCOMPARE(data.size(), 1);
    }

    {
        QByteArray copyData2 = copyData;

        copyData.push_back(0xFF);

        QVERIFY(*static_cast<Client_RX::RESPONSE_DEV_LIST_PKG*>(Client_RX::accumPacket(copyData)) == pkg);
        QCOMPARE(copyData.size(), 1);

        copyData2 = copyData2.mid(0, copyData2.size() - 3);
        QVERIFY(static_cast<Client_RX::RESPONSE_DEV_LIST_PKG*>(Client_RX::accumPacket(copyData2))->type == Client_RX::Response_Type::RESPONSE_INCOMPLETE);
        QCOMPARE(copyData2.size(), 79);
    }
}

void TEST_IOTV_SC::Client_RX__createResponse_STATE_PKG()
{
    QString devName = "Test dev name 1234";
    bool state = true;

    QByteArray data;
    data.push_back((devName.size() << 3) | IOTV_SC::RESPONSE_STATE_FIRST_BYTE);
    data.push_back((state << 5) | IOTV_SC::RESPONSE_STATE_SECOND_BYTE);
    data.append(devName.toLocal8Bit());

    Client_RX::RESPONSE_STATE_PKG pkg;
    pkg.name = devName;
    pkg.state = state;

    Client_RX::RESPONSE_STATE_PKG *resPkg = static_cast<Client_RX::RESPONSE_STATE_PKG*>(Client_RX::accumPacket(data));

    QCOMPARE(*resPkg, pkg);
    pkg.state = false;
    QVERIFY(!(*resPkg == pkg));

    delete resPkg;
}

void TEST_IOTV_SC::Client_RX__createResponse_READ_PKG()
{
    QString devName = "T";
    uint8_t channelNumber = 0;

    QByteArray data, rawData;
    rawData.push_back(1);
    rawData.push_back(2);
    rawData.push_back(3);
    rawData.push_back(4);
    rawData.push_back(5);

    data.push_back((devName.size() << 3) | IOTV_SC::RESPONSE_READ_BYTE);
    data.push_back(channelNumber);
    data.push_back(rawData.size() >> 8);
    data.push_back(rawData.size());
    data.append(devName.toLocal8Bit());
    data.append(rawData);

    Client_RX::RESPONSE_READ_PKG pkg;
    pkg.name = devName;
    pkg.channelNumber = channelNumber;
    pkg.data = rawData;

    QCOMPARE(*static_cast<Client_RX::RESPONSE_READ_PKG*>(Client_RX::accumPacket(data)), pkg);
}

void TEST_IOTV_SC::Client_RX__createResponse_WRITE_PKG()
{
    QString devName = "Test dev name 123";
    uint8_t channelNumber = 15;

    QByteArray data;
    data.push_back((devName.size() << 3) | IOTV_SC::RESPONSE_WRITE_BYTE);
    data.push_back(channelNumber);
    data.append(devName.toLocal8Bit());

    Client_RX::RESPONSE_WRITE_PKG pkg;
    pkg.name = devName;
    pkg.channelNumber = channelNumber;

    QCOMPARE(*static_cast<Client_RX::RESPONSE_WRITE_PKG*>(Client_RX::accumPacket(data)), pkg);
}

QTEST_APPLESS_MAIN(TEST_IOTV_SC)

#include "tst_test_iotv_sc.moc"
