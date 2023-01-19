#include <QtTest>

#include "iotvp_creator.h"
#include "iotvp_state.h"

class IOTVP_State_Test : public QObject
{
    Q_OBJECT

public:
    IOTVP_State_Test();
    ~IOTVP_State_Test();

    IOTVP_State pkg;

private slots:
    void test_name();
    void test_state();
    void test_flags();
    void test_checkSum();
    void test_Pkgdata();
    void test_createPkgFromData();
};

IOTVP_State_Test::IOTVP_State_Test()
{
    pkg.setName("new Name");
    pkg.setState(IOTVP_State::STATE::ONLINE);
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

IOTVP_State_Test::~IOTVP_State_Test()
{

}

void IOTVP_State_Test::test_name()
{
    QCOMPARE(pkg.name(), "new Name");
    QCOMPARE(pkg.nameSize(), pkg.name().size());
    QCOMPARE(pkg.nameSize(), 8);

    pkg.setName({500, 'A'});
    QCOMPARE(pkg.nameSize(), 255);
}

void IOTVP_State_Test::test_state()
{
    QCOMPARE(pkg.state(), IOTVP_State::STATE::ONLINE);
    QCOMPARE(static_cast<uint8_t>(pkg.state()), 1);
    QCOMPARE(static_cast<uint8_t>(pkg.state()), static_cast<uint8_t>(IOTVP_State::STATE::ONLINE));
}

void IOTVP_State_Test::test_flags()
{
    QCOMPARE(pkg.flags(), IOTVP_AbstractBody::FLAGS::ERROR);
    QCOMPARE(static_cast<uint8_t>(pkg.flags()), 0xFF);
}

void IOTVP_State_Test::test_checkSum()
{
    QCOMPARE(pkg.checkSum(), pkg.nameSize() + static_cast<uint8_t>(pkg.state()) + static_cast<uint8_t>(pkg.flags()) + pkg.dataSize());
}

void IOTVP_State_Test::test_Pkgdata()
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

void IOTVP_State_Test::test_createPkgFromData()
{

    const char dataRaw[] =  {
                                '\0',                                           // Длина имени
                                '\0',                                           // Состояние устройства
                                '\0',                                           // Флаги
                                '\0', '\0', '\0', '\0',                         // Размер пакета данных
                                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'  // Контрольная сумма тела пакета
                            };
    QByteArray data(dataRaw, 15);
    bool complete, error;
    auto pair = IOTVP_Creator::createPkgs(data, complete, error);


}

QTEST_APPLESS_MAIN(IOTVP_State_Test)

#include "tst_iotvp_state_test.moc"
