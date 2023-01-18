#include <QtTest>

#include "iotvp_read_write.h"

class IOTVP_ReadWrite_Test : public QObject
{
    Q_OBJECT

    friend class IOTVP_READ_WRITE;

public:
    IOTVP_ReadWrite_Test();
    ~IOTVP_ReadWrite_Test();

    IOTVP_READ_WRITE pkg;

private slots:
    void test_name();
    void test_channel();
    void test_flags();
    void test_checkSum();
    void test_Pkgdata();


};

IOTVP_ReadWrite_Test::IOTVP_ReadWrite_Test()
{
    pkg.setName("new Name");
    pkg.setChannelNumber(5);
    pkg.setFlags(IOTVP_AbstractBody::FLAGS::ERROR);

    QByteArray data;
    data.push_back(1);
    data.push_back(2);
    data.push_back('\0');
    data.push_back(3);
    data.push_back('\0');
    pkg.setData(data);

    qDebug() << "Name: " << pkg.name();
    qDebug() << "dataSize: " << pkg.dataSize();
    qDebug() << "pkg size: " << pkg.size();
    qDebug() << "checkSum: " << pkg.checkSum();
    qDebug() << "pkg to hex data: " << pkg.toData().toHex(':');

    pkg.setData({});
}

IOTVP_ReadWrite_Test::~IOTVP_ReadWrite_Test()
{

}

void IOTVP_ReadWrite_Test::test_name()
{
    pkg.setName("new Name");
    QCOMPARE(pkg.name(), "new Name");
    QCOMPARE(pkg.nameSize(), pkg.name().size());
    QCOMPARE(pkg.nameSize(), 8);

    pkg.setName({300, 'A'});
    QCOMPARE(pkg.nameSize(), 255);
}

void IOTVP_ReadWrite_Test::test_channel()
{
    QCOMPARE(pkg.channelNumber(), 5);
}

void IOTVP_ReadWrite_Test::test_flags()
{
    QCOMPARE(pkg.flags(), IOTVP_AbstractBody::FLAGS::ERROR);
    QCOMPARE(static_cast<uint8_t>(pkg.flags()), 0xFF);
}

void IOTVP_ReadWrite_Test::test_checkSum()
{
    QCOMPARE(pkg.checkSum(), pkg.nameSize() + pkg.channelNumber() + static_cast<uint8_t>(pkg.flags()) + pkg.dataSize());
}

void IOTVP_ReadWrite_Test::test_Pkgdata()
{
    QCOMPARE(pkg.dataSize(), 0);

    {
        QByteArray data(255, 'A');
        pkg.setData(data);
    }
    QCOMPARE(pkg.dataSize(), 255);

    {
        QByteArray data(512, 'A');
        pkg.setData(data);
    }
    QCOMPARE(pkg.dataSize(), 512);
}

QTEST_APPLESS_MAIN(IOTVP_ReadWrite_Test)

#include "tst_iotvp_readwrite.moc"
