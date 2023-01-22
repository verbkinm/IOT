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
    void test_headerOK();
    void test_headerNotFull();
    void test_bodyNotFull();
    void test_checkSum();

    void test_bodyStateNotFull();
    void test_bodyStateWithoutName();
    void test_bodyStateWithtName();

    void test_bodyReadNotFull();
    void test_bodyReadWithoutName();
    void test_bodyReadWithtName();

    void test_bodyIdetification();

    void test_equally();
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

}

void IOTVP_Header_Test::test_headerOK()
{
    // Цельный заголовок без ошибок и без тела пакета
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        5,                                           // Назначение пакета PING
        1,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 0, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 9  // Контрольная сумма тела пакета
    };

    QByteArray data(dataRaw, 20);
    IOTVP_Creator creator(data);
    creator.createPkgs();
    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize() == 0, true);
    QCOMPARE(creator.cutDataSize(), 20); //IOTVP_Abstract::HEADER_SIZE

    auto header = creator.takeHeader();
    QCOMPARE(header->toData(), data);
}

void IOTVP_Header_Test::test_headerNotFull()
{
    // Заголовок не полный
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        2,                                           // Назначение пакета PING
        1,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', // Размер тела пакета
    };

    QByteArray data(dataRaw, 12);
    IOTVP_Creator creator(data);
    creator.createPkgs();
    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize() > 0, true);

    auto header = creator.takeHeader();
    QCOMPARE(header, nullptr);
}

void IOTVP_Header_Test::test_bodyNotFull()
{
    // Тело пакета не всё пришло
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        2,                                           // Назначение пакета STATE
        1,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 7  // Контрольная сумма тела пакета
    };

    QByteArray data(dataRaw, 20);
    IOTVP_Creator creator(data);
    creator.createPkgs();
    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize() > 0, true);
}

void IOTVP_Header_Test::test_checkSum()
{
    // checkSum error
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        2,                                           // Назначение пакета PING
        1,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 9  // Контрольная сумма тела пакета
    };

    QByteArray data(dataRaw, 20);
    IOTVP_Creator creator(data);
    creator.createPkgs();
    QCOMPARE(creator.error(), true);
}

void IOTVP_Header_Test::test_bodyStateNotFull()
{
    // Заголовок и тело State - OK
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        2,                                           // Назначение пакета STATE
        1,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 15, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 21,  // Контрольная сумма тела пакета

        '\0',                                       // Длина имени
        1,                                          // Состояние устройства 1 - доступно
        '\0',                                       // Флаги
        '\0', '\0', '\0', 1,                        // Размер пакета данных
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 2 // Контрольная сумма тела пакета
    };

    QByteArray data(dataRaw, 35);
    IOTVP_Creator creator(data);
    creator.createPkgs();
    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize(), 36);

    auto header = creator.takeHeader();
    QCOMPARE(header->toData() != data.sliced(0, 20), true);

}

void IOTVP_Header_Test::test_bodyStateWithoutName()
{
    // Заголовок и тело State - OK
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        2,                                           // Назначение пакета STATE
        static_cast<char>(0xFF),                     // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 15, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', 1, 0x13,  // Контрольная сумма тела пакета 275

        '\0',                                       // Длина имени
        1,                                          // Состояние устройства 1 - доступно
        '\0',                                       // Флаги
        '\0', '\0', '\0', '\0',                     // Размер пакета данных
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 1  // Контрольная сумма тела пакета
    };

    QByteArray data(dataRaw, 35);
    IOTVP_Creator creator(data);
    creator.createPkgs();

    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize(), 0);
    QCOMPARE(creator.cutDataSize(), 35); // 35 = 20 + 15 = размер заголовка + тело State

    auto header = creator.takeHeader();
    QCOMPARE(header->toData(),  data);

    auto body = header->takeBody();
    QCOMPARE(body->toData(),  data.sliced(20));
}

void IOTVP_Header_Test::test_bodyStateWithtName()
{
    // Заголовок и тело State - OK
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        2,                                           // Назначение пакета STATE
        0,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 19, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 24,  // Контрольная сумма тела пакета

        4,                                          // Длина имени
        1,                                          // Состояние устройства 1 - доступно
        static_cast<char>(0xFF),                    // Флаги
        '\0', '\0', '\0', '\0',                     // Размер пакета данных
        '\0', '\0', '\0', '\0', '\0', '\0', 1, 0x04,// Контрольная сумма тела пакета  = 300
        'N', 'a', 'm', 'e'
    };

    QByteArray data(dataRaw, 39);
    IOTVP_Creator creator(data);
    creator.createPkgs();

    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize(), 0);
    QCOMPARE(creator.cutDataSize(), 39); // 35 = 20 + 15 + 4 = размер заголовка + тело State + имя устройства из State

    auto header = creator.takeHeader();
    QCOMPARE(header->toData(), data);

    auto body = header->takeBody();
    QCOMPARE(body->toData(),  data.sliced(20));

    QCOMPARE(body->name(), "Name");

    IOTVP_State state;
    state.setName("Name");
    state.setState(IOTVP_State::STATE::ONLINE);
    state.setFlags(IOTVP_State::FLAGS::ERROR);

    IOTVP_State *ptr = dynamic_cast<IOTVP_State*>(body.get());
    QCOMPARE(state, *ptr);
}

void IOTVP_Header_Test::test_bodyReadNotFull()
{
    // Заголовок и тело ReadWrite - OK
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        3,                                           // Назначение пакета Read
        0,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 15, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 21,  // Контрольная сумма тела пакета

        '\0',                                       // Длина имени
        5,                                          // Номер канала
        '\0',                                       // Флаги
        '\0', '\0', '\0', 1,                        // Размер пакета данных
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 6  // Контрольная сумма тела пакета
    };

    QByteArray data(dataRaw, 35);
    IOTVP_Creator creator(data);
    creator.createPkgs();
    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize(), 36);

    auto header = creator.takeHeader();
    QCOMPARE(header->toData() != data.sliced(0, 20), true);
}

void IOTVP_Header_Test::test_bodyReadWithoutName()
{
    // Заголовок и тело ReadWrite - OK
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        3,                                           // Назначение пакета Read
        0,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 15, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 21,  // Контрольная сумма тела пакета

        '\0',                                       // Длина имени
        5,                                          // Номер канала
        '\0',                                       // Флаги
        '\0', '\0', '\0', 0,                        // Размер пакета данных
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 5 // Контрольная сумма тела пакета
    };

    QByteArray data(dataRaw, 35);
    IOTVP_Creator creator(data);
    creator.createPkgs();
    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize(), 0);

    auto header = creator.takeHeader();
    QCOMPARE(header->toData() != data.sliced(0, 20), true);
    QCOMPARE(header->toData(), data);

    auto body = header->takeBody();
    QCOMPARE(body->name().isEmpty(), true);
}

void IOTVP_Header_Test::test_bodyReadWithtName()
{
    {
        // Заголовок и тело ReadWrite - Error
        const char dataRaw[] =  {
            2,                                           // Версия протокола
            1,                                           // Тип пакета - запрос
            3,                                           // Назначение пакета Read
            0,                                           // Флаги
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 15, // !!! Размер тела пакета меньше чем body.size + body.nameSize + body.dataSize
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 21,  // Контрольная сумма тела пакета

            5,                                          // !!! Длина имени. То есть пакет body должен быть 15 (документация) + 5 (длина имени), а в header пришло, что размер тела заголовка равен 15, что на 5 меньше
            5,                                          // Номер канала
            '\0',                                       // Флаги
            '\0', '\0', '\0', 0,                        // Размер пакета данных
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 10, // Контрольная сумма тела пакета
            'N', 'a', 'm', 'e', '!'
        };

        QByteArray data(dataRaw, 40);
        IOTVP_Creator creator(data);
        creator.createPkgs();
        QCOMPARE(creator.error(), true);
        QCOMPARE(creator.expectedDataSize(), 0);
        QCOMPARE(creator.cutDataSize(), 0);

        auto header = creator.takeHeader();
        QCOMPARE(header->toData() != data.sliced(0, 20), true);
        QCOMPARE(header->toData().size(), data.size());
        QCOMPARE(header->toData() != data, true);

        auto body = header->takeBody();
        QCOMPARE(body->name(), "Name!");
    }
    {
        // Заголовок и тело ReadWrite - OK
        const char dataRaw[] =  {
            2,                                           // Версия протокола
            1,                                           // Тип пакета - запрос
            3,                                           // Назначение пакета Read
            0,                                           // Флаги
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 20, // Размер тела пакета
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 26,  // Контрольная сумма тела пакета
            //*****************************************************************************
            5,                                          // Длина имени
            5,                                          // Номер канала
            '\0',                                       // Флаги
            '\0', '\0', '\0', 0,                        // Размер пакета данных
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 10, // Контрольная сумма тела пакета
            'N', 'a', 'm', 'e', '!'
        };

        QByteArray data(dataRaw, 40);
        IOTVP_Creator creator(data);
        creator.createPkgs();
        QCOMPARE(creator.error(), false);
        QCOMPARE(creator.expectedDataSize(), 0);
        QCOMPARE(creator.cutDataSize(), 40);

        auto header = creator.takeHeader();
        QCOMPARE(header->toData() != data.sliced(0, 20), true);
        QCOMPARE(header->toData(), data);

        auto body = header->takeBody();
        QCOMPARE(body->name(), "Name!");
    }
}

void IOTVP_Header_Test::test_bodyIdetification()
{
    {
        // Заголовок и тело Identification  - OK
        const char dataRaw[] =  {
            2,                                           // Версия протокола
            1,                                           // Тип пакета - запрос
            1,                                           // Назначение пакета Identification
            0,                                           // Флаги
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 34, // Размер тела пакета
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 38,  // Контрольная сумма тела пакета
            //*****************************************************************************
            '\0',                                        // id
            1,                                           // id
            4,                                           // Длина имени
            '\0',                                        // Длина описания устройства
            11,                                          // Длина описания устройства
            1,                                           // Кол-во каналов записи
            2,                                           // Кол-во каналов чтения
            0,                                           // Флаги
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 19,// Контрольная сумма тела пакета
            'N', 'a', 'm', 'e',                          // Имя устройства
            'D','e','s','c','r','i','p','t','i','o','n', // Описания устройства
            '\0',                                        // Типы данных кля канала записи
            6, 6                                         // Типы данных кля канала чтения
        };

        QByteArray data(dataRaw, 54);
        IOTVP_Creator creator(data);
        creator.createPkgs();
        QCOMPARE(creator.error(), false);
        QCOMPARE(creator.expectedDataSize(), 0);
        QCOMPARE(creator.cutDataSize(), 54);

        auto header = creator.takeHeader();
        QCOMPARE(header->toData() != data.sliced(0, 20), true);
        QCOMPARE(header->toData(), data);

        auto body = header->takeBody();
        QCOMPARE(body->name(), "Name");
    }
    {
        // Заголовок и тело Identification - ERROR
        const char dataRaw[] =  {
            2,                                           // Версия протокола
            1,                                           // Тип пакета - запрос
            1,                                           // Назначение пакета Identification
            0,                                           // Флаги
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 34,// Размер тела пакета
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 38,// Контрольная сумма тела пакета
            //*****************************************************************************
            '\0',                                        // id
            1,                                           // id
            4,                                           // Длина имени
            '\0',                                        // Длина описания устройства
            11,                                          // Длина описания устройства
            1,                                           // Кол-во каналов записи
            2,                                           // Кол-во каналов чтения
            0,                                           // Флаги
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', 19,// Контрольная сумма тела пакета
            'N', 'a', 'm', 'e',                          // Имя устройства
            'D','e','s','c','r','i','p','t','i','o','n', // Описания устройства
            '\0',                                        // Типы данных кля канала записи
            6, 6                                         // Типы данных кля канала чтения
        };

        QByteArray data(dataRaw, 55); // 54
        IOTVP_Creator creator(data);
        creator.createPkgs();
        QCOMPARE(creator.error(), false);
        QCOMPARE(creator.expectedDataSize(), 0);
        QCOMPARE(creator.cutDataSize(), 54);

        auto header = creator.takeHeader();
        QCOMPARE(header->toData() != data.sliced(0, 20), true);
        QCOMPARE(header->toData() != data, true);

        auto body = header->takeBody();
        QCOMPARE(body->name(), "Name");
    }
}

void IOTVP_Header_Test::test_equally()
{
    // Цельный заголовок без ошибок и без тела пакета
    const char dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        5,                                           // Назначение пакета PING
        static_cast<char>(0xff),                     // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 0, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', 1, 7  // Контрольная сумма тела пакета 107
    };

    QByteArray data(dataRaw, 20);
    IOTVP_Creator creator(data);
    creator.createPkgs();
    QCOMPARE(creator.error(), false);
    QCOMPARE(creator.expectedDataSize() == 0, true);
    QCOMPARE(creator.cutDataSize(), 20); //IOTVP_Abstract::HEADER_SIZE

    auto header = creator.takeHeader();
    QCOMPARE(header->toData(), data);

    IOTVP_Header header2;
    header2.setVersion(2);
    header2.setType(IOTVP_Header::TYPE::REQUEST);
    header2.setAssignment(IOTVP_Header::ASSIGNMENT::PING_PONG);
    header2.setFlags(IOTVP_Header::FLAGS::ERROR);

    QCOMPARE(*header, header2);
}

QTEST_APPLESS_MAIN(IOTVP_Header_Test)

#include "tst_iotvp_header_test.moc"
