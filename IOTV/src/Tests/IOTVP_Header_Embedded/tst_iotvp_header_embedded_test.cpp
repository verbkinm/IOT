#include <QtTest>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server.h"

class IOTVP_Header_Embedded_Test : public QObject
{
    Q_OBJECT

public:
    IOTVP_Header_Embedded_Test();
    ~IOTVP_Header_Embedded_Test();

    IOTV_Server iot = {
        .id = 1,
        .name = "Demo name",
        .description = "Switch",
        .readChannel = {false, false, true},
        .readChannelType = {DATA_TYPE_INT_16, DATA_TYPE_BOOL, DATA_TYPE_BOOL},
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
    void test_dataRecivedIdentofocation();
    void test_dataRecivedRead();

    void test_DataTransmitPing();
    void test_dataTransmitIdentofocation();
    void test_dataTransmitRead();

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

    const struct Header *header = createHeader(dataRaw, 20, &error, &expextedDataSize, &cutDataSize);
    QCOMPARE(error, false);
    QCOMPARE(expextedDataSize, 0);
    QCOMPARE(cutDataSize, 20);
    QCOMPARE(header->type, Header::HEADER_TYPE_REQUEST);
    QCOMPARE(header->assignment, Header::HEADER_ASSIGNMENT_PING_PONG);
    QCOMPARE(header->flags, 1);
    QCOMPARE(header->dataSize, 0);
    QCOMPARE(headerCheckSum(header), 9);
    QCOMPARE(isLittleEndian(), Q_BYTE_ORDER == Q_LITTLE_ENDIAN);

    char outData[20];
    headerToData(header, outData, 20);

    for (int i = 0; i < 20; ++i)
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

void IOTVP_Header_Embedded_Test::test_dataRecivedIdentofocation()
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
        Header::HEADER_ASSIGNMENT_READ,             // Назначение пакета Indetification
        Header::HEADER_FLAGS_NONE,                  // Флаги
        0, 0, 0, 0, 0, 0, 0, 21,                    // Размер тела пакета               21
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
    QCOMPARE(realBufSize, 41);
    QCOMPARE(cutDataSize, 41);
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

    for (int i = 0; i < 20; ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitIdentofocation()
{
    test_dataRecivedIdentofocation();

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
}

void IOTVP_Header_Embedded_Test::test_dataTransmitRead()
{
    test_dataRecivedRead();

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .channelNumber = 1,
        .dataSize = 0,
        .name = iot.name,
        .data = NULL
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_READ,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = HEADER_SIZE + readWriteSize(&readWrite),
        .identification = NULL,
        .readWrite = &readWrite,
        .state = NULL
    };

    char outData[headerSize(&header)];

    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

}

QTEST_APPLESS_MAIN(IOTVP_Header_Embedded_Test)

#include "tst_iotvp_header_embedded_test.moc"
