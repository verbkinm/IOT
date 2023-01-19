#include <QtTest>

#include "iotvp_header.h"
#include "iotvp_creator.h"

class IOTVP_Header_Test : public QObject
{
    Q_OBJECT

public:
    IOTVP_Header_Test();
    ~IOTVP_Header_Test();

    void printDebug();

    IOTVP_Header header;

private slots:
    void test_addBodyState();
    void test_createPkgFromData();
};

IOTVP_Header_Test::IOTVP_Header_Test()
{
    header.setVersion(2);
    header.setType(IOTVP_Header::TYPE::REQUEST);
    header.setAssignment(IOTVP_Header::ASSIGNMENT::IDENTIFICATION);
    header.setFlags(IOTVP_Header::FLAGS::ERROR);
    //    std::unique_ptr<IOTVP_AbstractBody> ptr = std::make_unique<IOTVP_Identification>();

    printDebug();
}

IOTVP_Header_Test::~IOTVP_Header_Test()
{

}

void IOTVP_Header_Test::printDebug()
{
    qDebug() << "Version: " << header.version();
    qDebug() << "Type: " << static_cast<uint8_t>(header.type());
    qDebug() << "Appointment: " << static_cast<uint8_t>(header.assignment());
    qDebug() << "Flags: " << static_cast<uint8_t>(header.flags());
    qDebug() << "data size: " << header.dataSize();
    qDebug() << "checkSum: " << header.checkSum();
    qDebug() << "size: " << header.size();
    qDebug() << "hex data: " << header.toData().toHex(':');
}

void IOTVP_Header_Test::test_addBodyState()
{
    std::unique_ptr<IOTVP_AbstractBody> ptr = std::make_unique<IOTVP_State>();
    header.setBody(std::move(ptr));
    printDebug();
}

void IOTVP_Header_Test::test_createPkgFromData()
{
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета
        5,                                           // Назначение пакета
        1,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 10  // Контрольная сумма тела пакета
    };

    QByteArray data(dataRaw, 20);
    bool complete, error;
    auto pair = IOTVP_Creator::createPkgs(data, complete, error);
}

QTEST_APPLESS_MAIN(IOTVP_Header_Test)

#include "tst_iotvp_header_test.moc"
