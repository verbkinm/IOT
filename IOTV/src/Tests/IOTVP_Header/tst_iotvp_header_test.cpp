#include <QtTest>

#include "iotvp_header.h"
#include "iotvp_state.h"

class IOTVP_Header_Test : public QObject
{
    Q_OBJECT

public:
    IOTVP_Header_Test();
    ~IOTVP_Header_Test();

    IOTVP_Header header;

private slots:
    void test_case1();
    void test_addBodyState();

};

IOTVP_Header_Test::IOTVP_Header_Test()
{
    header.setVersion(2);
    header.setType(IOTVP_Header::TYPE::REQUEST);
    header.setAppointment(IOTVP_Header::APPOINTMRNT::IDENTIFICATION);
    header.setFlags(IOTVP_Header::FLAGS::ERROR);
//    std::unique_ptr<IOTVP_AbstractBody> ptr = std::make_unique<IOTVP_Identification>();

    qDebug() << "Version: " << header.version();
    qDebug() << "Type: " << static_cast<uint8_t>(header.type());
    qDebug() << "Appointment: " << static_cast<uint8_t>(header.appointment());
    qDebug() << "Flags: " << static_cast<uint8_t>(header.flags());
    qDebug() << "data size: " << header.bodySize();
    qDebug() << "checkSum: " << header.checkSum();
    qDebug() << "size: " << header.size();
    qDebug() << "hex data: " << header.toData().toHex(':');
}

IOTVP_Header_Test::~IOTVP_Header_Test()
{

}

void IOTVP_Header_Test::test_case1()
{

}

void IOTVP_Header_Test::test_addBodyState()
{
    std::unique_ptr<IOTVP_AbstractBody> ptr = std::make_unique<IOTVP_State>();
    header.setBody(std::move(ptr));
    qDebug() << "data size: " << header.bodySize();
    qDebug() << "checkSum: " << header.checkSum();
    qDebug() << "size: " << header.size();
    qDebug() << "hex data: " << header.toData().toHex(':');
}

QTEST_APPLESS_MAIN(IOTVP_Header_Test)

#include "tst_iotvp_header_test.moc"
