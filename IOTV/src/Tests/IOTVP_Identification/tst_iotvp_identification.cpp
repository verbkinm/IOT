#include <QtTest>

#include "iotvp_identification.h"

class IOTVP_Identification_Test : public QObject
{
    Q_OBJECT

public:
    IOTVP_Identification_Test();
    ~IOTVP_Identification_Test();

    IOTVP_Identification pkg;

private slots:
    void test_name();
    void test_description();
    void test_flags();
    void test_checkSum();
    void test_Pkgdata();
    void test_channels();
};

IOTVP_Identification_Test::IOTVP_Identification_Test()
{
    pkg.setId(1);
    pkg.setName("");
    pkg.setDescription("");
    pkg.setFlags(IOTVP_AbstractBody::FLAGS::ERROR);

    qDebug() << "ID: " << pkg.id();
    qDebug() << "Name: " << pkg.name();
    qDebug() << "Description: " << pkg.description();
    qDebug() << "dataSize: " << pkg.dataSize();
    qDebug() << "pkg size: " << pkg.size();
    qDebug() << "checkSum: " << pkg.checkSum();
    qDebug() << "pkg to hex data: " << pkg.toData().toHex(':');
}

IOTVP_Identification_Test::~IOTVP_Identification_Test()
{

}

void IOTVP_Identification_Test::test_name()
{
    pkg.setName("new Name");
    QCOMPARE(pkg.name(), "new Name");
    QCOMPARE(pkg.nameSize(), pkg.name().size());
    QCOMPARE(pkg.nameSize(), 8);

    QString newName(300, 'A');
    pkg.setName(newName);
    QCOMPARE(pkg.nameSize(), 255);

    pkg.setName("123 \0 567");
    QCOMPARE(pkg.nameSize(), 4);

    pkg.setName("Абвгдеёжз");
    QCOMPARE(pkg.nameSize(), 18);
}

void IOTVP_Identification_Test::test_description()
{
    QCOMPARE(pkg.description(), "");
    QCOMPARE(pkg.descriptionSize(), 0);

    pkg.setDescription("Абвгдеёжз");
    QCOMPARE(pkg.descriptionSize(), 18);

    pkg.setDescription("123 \0 567");
    QCOMPARE(pkg.descriptionSize(), 4);

    QString newDescription(65600, 'A');
    pkg.setDescription(newDescription);
    QCOMPARE(pkg.descriptionSize(), 65535);
}

void IOTVP_Identification_Test::test_flags()
{
    QCOMPARE(pkg.flags(), IOTVP_AbstractBody::FLAGS::ERROR);
    QCOMPARE(static_cast<uint8_t>(pkg.flags()), 0xFF);
}

void IOTVP_Identification_Test::test_checkSum()
{
    auto chSum = pkg.checkSum();
    QCOMPARE(chSum, pkg.id() + pkg.nameSize() + pkg.descriptionSize() + pkg.numberWriteChannel() + pkg.numberReadChannel() + static_cast<uint8_t>(pkg.flags()));

    QList<Raw::DATA_TYPE> channel(10);
    pkg.setWriteChannel(channel);

    QCOMPARE(chSum + channel.size(), pkg.id() + pkg.nameSize() + pkg.descriptionSize() + pkg.numberWriteChannel() + pkg.numberReadChannel() + static_cast<uint8_t>(pkg.flags()));
}

void IOTVP_Identification_Test::test_Pkgdata()
{
    QCOMPARE(pkg.dataSize(), 0);
}

void IOTVP_Identification_Test::test_channels()
{
    {
        QList<Raw::DATA_TYPE> channel(10);
        pkg.setWriteChannel(channel);

        QCOMPARE(pkg.numberReadChannel(), 0);
        QCOMPARE(pkg.numberWriteChannel(), channel.size());

        QCOMPARE(pkg.toData().at(5), pkg.numberWriteChannel());
        QCOMPARE(pkg.toData().at(5), channel.size());
        QCOMPARE(pkg.toData().at(6), pkg.numberReadChannel());
    }

    {
        QList<Raw::DATA_TYPE> channel(11);
        pkg.setReadChannel(channel);

        QCOMPARE(pkg.numberReadChannel(), channel.size());
        QCOMPARE(pkg.numberWriteChannel(), 10);

        QCOMPARE(pkg.toData().at(5), pkg.numberWriteChannel());
        QCOMPARE(pkg.toData().at(6), channel.size());
    }

    pkg.setReadChannel({});
    pkg.setWriteChannel({});

    QCOMPARE(pkg.numberReadChannel(), 0);
    QCOMPARE(pkg.numberWriteChannel(), 0);

    QList<Raw::DATA_TYPE> channel(300);
    pkg.setWriteChannel(channel);
    pkg.setReadChannel(channel);

    QCOMPARE(pkg.numberReadChannel(), 255);
    QCOMPARE(pkg.numberWriteChannel(), 255);
}

QTEST_APPLESS_MAIN(IOTVP_Identification_Test)

#include "tst_iotvp_identification.moc"
