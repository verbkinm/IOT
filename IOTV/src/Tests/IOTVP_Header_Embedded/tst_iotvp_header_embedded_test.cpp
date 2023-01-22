#include <QtTest>

#include "creatorpkgs.h"

class IOTVP_Header_Embedded_Test : public QObject
{
    Q_OBJECT

public:
    IOTVP_Header_Embedded_Test();
    ~IOTVP_Header_Embedded_Test();

    bool error = false;
    uint64_t expectedSize = 0;
    uint64_t cutDataSize = 0;

private slots:
    void test_headerOk();

};

IOTVP_Header_Embedded_Test::IOTVP_Header_Embedded_Test()
{

}

IOTVP_Header_Embedded_Test::~IOTVP_Header_Embedded_Test()
{

}

void IOTVP_Header_Embedded_Test::test_headerOk()
{
    // Цельный заголовок без ошибок и без тела пакета
    uint8_t dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        5,                                           // Назначение пакета PING
        1,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 0, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 9  // Контрольная сумма тела пакета
    };

//    uint8_t* ptr = (uint8_t *)dataRaw;
//    struct Header* header = createHeader(ptr, 20, &error, &expectedSize, &cutDataSize);
    QCOMPARE(error, false);

    QCOMPARE(isLittleEndian(), Q_BYTE_ORDER == Q_LITTLE_ENDIAN);
}

QTEST_APPLESS_MAIN(IOTVP_Header_Embedded_Test)

#include "tst_iotvp_header_embedded_test.moc"
