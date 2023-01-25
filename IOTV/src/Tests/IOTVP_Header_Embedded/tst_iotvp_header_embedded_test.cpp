#include <QtTest>
#include <QtEndian>
#include <QtCore>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server.h"

#include "iotvp_creator.h"

class IOTVP_Header_Embedded_Test : public QObject
{
    Q_OBJECT

public:
    IOTVP_Header_Embedded_Test();
    ~IOTVP_Header_Embedded_Test();

    IOTV_Server iot = {
        .id = 1,
        .name = "Device",
        .description = "Switch",
        .readChannel = {1, 55, 1},
        .readChannelType = {DATA_TYPE_INT_32, DATA_TYPE_INT_32, DATA_TYPE_INT_32},
        .writeChannelType = {DATA_TYPE_BOOL, DATA_TYPE_BOOL, DATA_TYPE_BOOL}
    };

    char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
    uint64_t realBufSize = 0;

    uint64_t expextedDataSize = HEADER_SIZE;
    uint64_t cutDataSize = 0;
    bool error = false;

    void dataRecived(char ch);

private slots:
    void test_headerOk();
    void test_dataRecivedPing();
    void test_dataRecivedIdentefication();
    void test_dataRecivedRead();
    void test_dataRecivedWrite();

    void test_DataTransmitPing();
    void test_dataTransmitIdentification();
    void test_dataTransmitRead();
    void test_dataTransmitWrite();

    void timeCompare();
};

IOTVP_Header_Embedded_Test::IOTVP_Header_Embedded_Test()
{

}

IOTVP_Header_Embedded_Test::~IOTVP_Header_Embedded_Test()
{

}

void IOTVP_Header_Embedded_Test::dataRecived(char ch)
{
    recivedBuffer[realBufSize] = ch;
    ++realBufSize;

    if (realBufSize < expextedDataSize)
        return;

    struct Header* header = createPkgs((uint8_t*)recivedBuffer, realBufSize, &error, &expextedDataSize, &cutDataSize);

    if (error == true)
    {
        realBufSize = 0;
        expextedDataSize = 0;
        cutDataSize = 0;
        clearHeader(header);
        return;
    }

    if (expextedDataSize > 0)
        return;

    if (header->type == Header::HEADER_TYPE_REQUEST)
    {
        if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
        {
           /* uint64_t size = */responseIdentificationData(transmitBuffer, BUFSIZ, &iot);
//            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == Header::HEADER_ASSIGNMENT_READ)
        {
           /* uint64_t size = */responseReadData(transmitBuffer, BUFSIZ, &iot, header);
//            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
        {
            /* uint64_t size = */responseWriteData(transmitBuffer, BUFSIZ, &iot, header);
 //            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == Header::HEADER_ASSIGNMENT_PING_PONG)
        {
            /*uint64_t size = */responsePingData(transmitBuffer, BUFSIZ);
//            socket->write(transmitBuffer, size);
        }
    }

    //!!!
//    memmove((void*)recivedBuffer, (void*)&recivedBuffer[cutDataSize], BUFSIZ - cutDataSize);
    realBufSize -= cutDataSize; // тут всегда должно уходить в ноль, если приём идёт по 1 байту!

    //страховка
    if (realBufSize >= BUFSIZ)
    {
        realBufSize = 0;
        expextedDataSize = 0;
        cutDataSize = 0;
    }

    clearHeader(header);
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

    const struct Header *header = createHeader(dataRaw, HEADER_SIZE, &error, &expextedDataSize, &cutDataSize);
    QCOMPARE(error, false);
    QCOMPARE(expextedDataSize, 0);
    QCOMPARE(cutDataSize, HEADER_SIZE);
    QCOMPARE(header->type, Header::HEADER_TYPE_REQUEST);
    QCOMPARE(header->assignment, Header::HEADER_ASSIGNMENT_PING_PONG);
    QCOMPARE(header->flags, 1);
    QCOMPARE(header->dataSize, 0);
    QCOMPARE(headerCheckSum(header), 9);
    QCOMPARE(isLittleEndian(), Q_BYTE_ORDER == Q_LITTLE_ENDIAN);

    char outData[HEADER_SIZE];
    headerToData(header, outData, HEADER_SIZE);

    for (uint i = 0; i < HEADER_SIZE; ++i)
        QCOMPARE(outData[i], dataRaw[i]);
}

void IOTVP_Header_Embedded_Test::test_dataRecivedPing()
{
    realBufSize = 0;
    error = false;
    expextedDataSize = 0;
    cutDataSize = 0;

    // Цельный заголовок без ошибок и без тела пакета
    uint8_t dataRaw[] =  {
        2,                                           // Версия протокола
        1,                                           // Тип пакета - запрос
        5,                                           // Назначение пакета PING
        1,                                           // Флаги
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 0, // Размер тела пакета
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', 9  // Контрольная сумма тела пакета
    };

    // в transmitBuffer будет ответ на Ping по окончанию цикла
    for (uint i = 0; i < HEADER_SIZE; ++i)
    {
        if (i > 0)
            QCOMPARE(expextedDataSize, HEADER_SIZE);

        QCOMPARE(realBufSize, i);
        QCOMPARE(cutDataSize, 0);
        QCOMPARE(error, false);

        dataRecived(dataRaw[i]);
    }

    QCOMPARE(expextedDataSize, 0);
    QCOMPARE(cutDataSize, 20);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(error, false);
}

void IOTVP_Header_Embedded_Test::test_dataRecivedIdentefication()
{
    realBufSize = 0;
    error = false;
    expextedDataSize = 0;
    cutDataSize = 0;

    // Цельный заголовок без ошибок и без тела пакета
    uint8_t dataRaw[] =  {
        2,                                          // Версия протокола
        Header::HEADER_TYPE_REQUEST             ,   // Тип пакета - запрос
        Header::HEADER_ASSIGNMENT_IDENTIFICATION,   // Назначение пакета Indetification
        Header::HEADER_FLAGS_ERROR,                 // Флаги
        0, 0, 0, 0, 0, 0, 0, 0,                     // Размер тела пакета
        0, 0, 0, 0, 0, 0, 1, 3                      // Контрольная сумма тела пакета  - 259
    };

    // в transmitBuffer будет ответ на Identification по окончанию цикла
    for (uint i = 0; i < HEADER_SIZE; ++i)
    {
        if (i > 0 && i < HEADER_SIZE)
            QCOMPARE(expextedDataSize, HEADER_SIZE);

        QCOMPARE(realBufSize, i);
        QCOMPARE(cutDataSize, 0);
        QCOMPARE(error, false);

        dataRecived(dataRaw[i]);
    }

    QCOMPARE(expextedDataSize, 0);
    QCOMPARE(cutDataSize, HEADER_SIZE);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(error, false);
}

void IOTVP_Header_Embedded_Test::test_dataRecivedRead()
{
    realBufSize = 0;
    error = false;
    expextedDataSize = 0;
    cutDataSize = 0;

    // Запрос чтения
    uint8_t dataRaw[] =  {
        2,                                          // Версия протокола
        Header::HEADER_TYPE_REQUEST,                // Тип пакета - запрос
        Header::HEADER_ASSIGNMENT_READ,             // Назначение пакета Read
        Header::HEADER_FLAGS_NONE,                  // Флаги
        0, 0, 0, 0, 0, 0, 0, 21,                    // Размер тела пакета               21 = READ_SIZE + nameSize
        0, 0, 0, 0, 0, 0, 0, 27,                    // Контрольная сумма тела пакета    27
        6,                                          // Длина имени устройства
        1,                                          // Номер канала
        0,                                          // Флаги
        0, 0, 0, 0,                                 // Размер данных
        0, 0, 0, 0, 0, 0, 0, 7,                     // Контрольная сумма
        'D', 'e', 'v', 'i', 'c', 'e'                // Имя устройства
    };

    // в transmitBuffer будет ответ на Read по окончанию цикла
    for (uint i = 0; i < HEADER_SIZE + READ_WRITE_SIZE + 6; ++i) // 6 - имя устройства
    {
        if (i > 0 && i < HEADER_SIZE)
            QCOMPARE(expextedDataSize, HEADER_SIZE);

//        if (i > HEADER_SIZE && i < HEADER_SIZE + READ_WRITE_SIZE)
//            QCOMPARE(expextedDataSize, HEADER_SIZE + READ_WRITE_SIZE + 6);

        QCOMPARE(error, false);
        dataRecived(dataRaw[i]);
    }

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(cutDataSize, 41);
    QCOMPARE(expextedDataSize, 0);

//    QCOMPARE(iot.readChannel[1], 123); //вызывается второй раз из теста write
}

void IOTVP_Header_Embedded_Test::test_dataRecivedWrite()
{
    realBufSize = 0;
    error = false;
    expextedDataSize = 0;
    cutDataSize = 0;

    // Запрос записи
    uint8_t dataRaw[] =  {
        2,                                          // Версия протокола
        Header::HEADER_TYPE_REQUEST,                // Тип пакета - запрос
        Header::HEADER_ASSIGNMENT_WRITE,            // Назначение пакета Write
        Header::HEADER_FLAGS_NONE,                  // Флаги
        0, 0, 0, 0, 0, 0, 0, 25,                    // Размер тела пакета               25 = Wite_SIZE + nameSize + data (4)
        0, 0, 0, 0, 0, 0, 0, 32,                    // Контрольная сумма тела пакета    32
        6,                                          // Длина имени устройства
        1,                                          // Номер канала
        0,                                          // Флаги
        0, 0, 0, 4,                                 // Размер данных
        0, 0, 0, 0, 0, 0, 0, 11,                    // Контрольная сумма
        'D', 'e', 'v', 'i', 'c', 'e',               // Имя устройства
        0, 0, 0, 123                                // Данные
    };

    // в transmitBuffer будет ответ на Write по окончанию цикла
    for (uint i = 0; i < HEADER_SIZE + READ_WRITE_SIZE + 6 + 4; ++i) // 6 - имя устройства, 4 - данные
    {
        if (i > 0 && i < HEADER_SIZE)
            QCOMPARE(expextedDataSize, HEADER_SIZE);

//        if (i > HEADER_SIZE && i < HEADER_SIZE + READ_WRITE_SIZE)
//            QCOMPARE(expextedDataSize, HEADER_SIZE + READ_WRITE_SIZE + 6);

        QCOMPARE(error, false);
        dataRecived(dataRaw[i]);
    }


    QCOMPARE(error, false);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(cutDataSize, 45);
    QCOMPARE(expextedDataSize, 0);

    QCOMPARE(iot.readChannel[1], 123);
}

void IOTVP_Header_Embedded_Test::test_DataTransmitPing()
{
    test_dataRecivedPing();
    // Заголовок ответ на PING
    struct Header header = {
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_PING_PONG,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = 0,
        .identification = NULL,
        .readWrite = NULL,
        .state = NULL
    };

    char outData[HEADER_SIZE];
    headerToData(&header, outData, HEADER_SIZE);

    for (uint i = 0; i < HEADER_SIZE; ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(cutDataSize, HEADER_SIZE);
    QCOMPARE(expextedDataSize, 0);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitIdentification()
{
    test_dataRecivedIdentefication();

    struct Identification ident = {
        .flags = Identification::Identification_FLAGS_NONE,
        .id = iot.id,
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .descriptionSize = static_cast<uint16_t>(strlen(iot.description)),
        .numberWriteChannel = WRITE_CHANNEL_LENGTH,
        .numberReadChannel = READ_CHANNEL_LENGTH,
        .name = iot.name,
        .description = iot.description,
        .writeChannelType = (const uint8_t *)&iot.writeChannelType,
        .readChannelType = (const uint8_t *)&iot.readChannelType
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_IDENTIFICATION,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = identificationSize(&ident),
        .identification = &ident,
        .readWrite = NULL,
        .state = NULL
    };

    char outData[headerSize(&header)];
    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(cutDataSize, HEADER_SIZE); // осталось от test_dataRecivedIdentefication
    QCOMPARE(expextedDataSize, 0);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitRead()
{
    // test_dataRecivedRead - кладёт в буфер данные для отправки на полученый пакет в тесте.
    // В отправляемых даных размер больше на .data
    test_dataRecivedRead();

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .channelNumber = 1,
        .dataSize = sizeof(iot.readChannel[0]),
        .name = iot.name,
        .data = (const uint8_t *)&iot.readChannel[1]
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_READ,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = readWriteSize(&readWrite),
        .identification = NULL,
        .readWrite = &readWrite,
        .state = NULL
    };

    char outData[headerSize(&header)];
    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(cutDataSize, HEADER_SIZE + READ_WRITE_SIZE + 6 /*Имя устройства*/); // осталось от test_dataTransmitRead
    QCOMPARE(expextedDataSize, 0);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitWrite()
{
    // test_dataRecivedWrite - кладёт в буфер данные для отправки на полученый пакет в тесте.
    test_dataRecivedWrite();

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .channelNumber = 1,
        .dataSize = sizeof(iot.readChannel[0]),
        .name = iot.name,
        .data = (const uint8_t *)&iot.readChannel[1]
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_WRITE,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = readWriteSize(&readWrite),
        .identification = NULL,
        .readWrite = &readWrite,
        .state = NULL
    };

    char outData[headerSize(&header)];
    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(cutDataSize, HEADER_SIZE + READ_WRITE_SIZE + 6 + 4); // осталось от test_dataRecivedWrite 45 = HEADER_SIZE + Wite_SIZE + nameSize + data (4)
    QCOMPARE(expextedDataSize, 0);
}

void IOTVP_Header_Embedded_Test::timeCompare()
{
    // Запрос записи
    uint8_t dataRaw[] =  {
        2,                                          // Версия протокола
        Header::HEADER_TYPE_REQUEST,                // Тип пакета - запрос
        Header::HEADER_ASSIGNMENT_WRITE,            // Назначение пакета Write
        Header::HEADER_FLAGS_NONE,                  // Флаги
        0, 0, 0, 0, 0, 0, 0, 25,                    // Размер тела пакета               25 = Wite_SIZE + nameSize + data (4)
        0, 0, 0, 0, 0, 0, 0, 32,                    // Контрольная сумма тела пакета    32
        6,                                          // Длина имени устройства
        1,                                          // Номер канала
        0,                                          // Флаги
        0, 0, 0, 4,                                 // Размер данных
        0, 0, 0, 0, 0, 0, 0, 11,                    // Контрольная сумма
        'D', 'e', 'v', 'i', 'c', 'e',               // Имя устройства
        0, 0, 0, 123                                // Данные
    };

    auto start = std::chrono::system_clock::now();

    realBufSize = 0;
    error = false;
    expextedDataSize = 0;
    cutDataSize = 0;

    for (int x = 0; x < 100'000; ++x)
    {
        // в transmitBuffer будет ответ на Write по окончанию цикла
        for (uint i = 0; i < HEADER_SIZE + READ_WRITE_SIZE + 6 + 4; ++i) // 6 - имя устройства, 4 - данные
            dataRecived(dataRaw[i]);
    }

    qDebug() << "IOTVP Embedded protocol - " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count();

    QByteArray rawData(reinterpret_cast<char*>(dataRaw), HEADER_SIZE + READ_WRITE_SIZE + 6 + 4);

    start = std::chrono::system_clock::now();

    for (int x = 0; x < 100'000; ++x)
    {
        IOTVP_Creator creator(rawData);
        creator.createPkgs();
        auto header = creator.takeHeader();
//        auto transmit = header->toData();
    }

    qDebug() << "IOTVP protocol - " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count();
}

QTEST_APPLESS_MAIN(IOTVP_Header_Embedded_Test)

#include "tst_iotvp_header_embedded_test.moc"
