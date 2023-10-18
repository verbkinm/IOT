#include <QtTest>
#include <QtEndian>
#include <QtCore>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"
#include "iotv_types.h"

const char *name = "Device";
const char *description = "Description";

uint64_t writeFunc(char *data, uint64_t dataSize, void *obj)
{
    Q_UNUSED(data)
    Q_UNUSED(dataSize)
    Q_UNUSED(obj)

    return 0;
}

class IOTVP_Header_Embedded_Test : public QObject
{
    Q_OBJECT

public:
    IOTVP_Header_Embedded_Test();
    ~IOTVP_Header_Embedded_Test();

    uint8_t readType[3] = {DATA_TYPE_INT_16, DATA_TYPE_INT_16, DATA_TYPE_INT_16};
    uint8_t writeType[3] = {DATA_TYPE_INT_16, DATA_TYPE_INT_16, DATA_TYPE_INT_16};

    struct IOTV_Server_embedded iot = {
        .id = 1,

        .numberReadChannel = 3,
        .numberWriteChannel = 2,

        .state = 1,

        .nameSize = static_cast<uint8_t>(strlen(name)),
        .descriptionSize = static_cast<uint16_t>(strlen(description)),

        .readChannel = NULL,
        .readChannelType = readType,
        .writeChannelType = writeType,

        .name = (char *)name,
        .description = (char *)description,
    };

    char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
    uint64_t realBufSize = 0;

    uint64_t expextedDataSize = HEADER_SIZE;
    uint64_t cutDataSize = 0;
    bool error = false;

    void dataRecived(char ch);
    void test_dataTransmitWriteFragmet();

private slots:
    void test_headerOk();
    void test_dataRecivedPing();
    void test_dataRecivedIdentefication();
    void test_dataRecivedRead();
    void test_dataRecivedWrite();
    void test_dataReciveState();

    void test_DataTransmitPing();
    void test_dataTransmitIdentification();
    void test_dataTransmitRead();
    void test_dataTransmitWrite();
    void test_dataTransmitState();

    void test_dataTransmitWriteFragmets();

    void test_pkgCount();
    void test_dataPart();
};

IOTVP_Header_Embedded_Test::IOTVP_Header_Embedded_Test()
{
    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded) * 3);

    iot.readChannel[0].dataSize = 2;
    iot.readChannel[1].dataSize = 2;
    iot.readChannel[2].dataSize = 2;

    iot.readChannel[0].data = (char *)malloc(iot.readChannel[0].dataSize);
    int16_t value = 257;
    memcpy(iot.readChannel[0].data, &value, iot.readChannel[0].dataSize);

    iot.readChannel[1].data = (char *)malloc(iot.readChannel[1].dataSize);
    //    value = 256;
    memcpy(iot.readChannel[1].data, &value, iot.readChannel[1].dataSize);

    iot.readChannel[2].data = (char *)malloc(iot.readChannel[2].dataSize);
    //    value = 256;
    memcpy(iot.readChannel[2].data, &value, iot.readChannel[2].dataSize);
}

IOTVP_Header_Embedded_Test::~IOTVP_Header_Embedded_Test()
{
    QFile file("test_dataTransmitWriteFragmets.txt");
    file.open(QIODevice::ReadWrite);
    QFile file2("test_dataTransmitWriteFragmets_2.txt");
    file2.open(QIODevice::ReadWrite);

    file.remove();
    file2.remove();
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

    if (header->type == HEADER_TYPE_REQUEST)
    {
        if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
        {
            /* uint64_t size = */responseIdentificationData(transmitBuffer, BUFSIZ, &iot);
            //            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == HEADER_ASSIGNMENT_READ)
        {
            /* uint64_t size = */responseReadData(transmitBuffer, BUFSIZ, &iot, header, writeFunc, NULL);
            //            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == HEADER_ASSIGNMENT_WRITE)
        {
            /* uint64_t size = */responseWriteData(transmitBuffer, BUFSIZ, &iot, header);
            //            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == HEADER_ASSIGNMENT_PING_PONG)
        {
            /*uint64_t size = */responsePingData(transmitBuffer, BUFSIZ);
            //            socket->write(transmitBuffer, size);
        }
        else if(header->assignment == HEADER_ASSIGNMENT_STATE)
        {
            /*uint64_t size = */responseStateData(transmitBuffer, BUFSIZ, &iot);
            //            socket->write(transmitBuffer, size);
        }
    }

    //!!!
    memmove((void*)recivedBuffer, (void*)&recivedBuffer[cutDataSize], BUFSIZ - cutDataSize);
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
        1, 0,                                        // Номер фрагмента
        1, 0,                                        // Кол-во фрагментов
        0, 0, 0, 0, 0, 0, 0, 0,                      // Размер тела пакета
        11, 0, 0, 0, 0, 0, 0, 0                      // Контрольная сумма тела пакета
    };

    const struct Header *header = createHeader(dataRaw, HEADER_SIZE, &error, &expextedDataSize, &cutDataSize);
    QCOMPARE(error, false);
    QCOMPARE(expextedDataSize, (uint64_t)0);
    QCOMPARE(cutDataSize, uint64_t(HEADER_SIZE));
    QCOMPARE(header->type, HEADER_TYPE_REQUEST);
    QCOMPARE(header->assignment, HEADER_ASSIGNMENT_PING_PONG);
    QCOMPARE(header->flags, 1);
    QCOMPARE(header->dataSize, (uint64_t)0);
    QCOMPARE(headerCheckSum(header), (uint64_t)11);
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
        1, 0,                                        // Номер фрагмента
        1, 0,                                        // Кол-во фрагментов
        0, 0, 0, 0, 0, 0, 0, 0,                      // Размер тела пакета
        11, 0, 0, 0, 0, 0, 0, 0                      // Контрольная сумма тела пакета
    };

    // в transmitBuffer будет ответ на Ping по окончанию цикла
    for (uint i = 0; i < HEADER_SIZE; ++i)
    {
        if (i > 0)
            QCOMPARE(expextedDataSize, (uint64_t)HEADER_SIZE);

        QCOMPARE(realBufSize, i);
        QCOMPARE(cutDataSize, (uint64_t)0);
        QCOMPARE(error, false);

        dataRecived(dataRaw[i]);
    }

    QCOMPARE(expextedDataSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)HEADER_SIZE);
    QCOMPARE(realBufSize, (uint64_t)0);
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
        HEADER_TYPE_REQUEST,                        // Тип пакета - запрос
        HEADER_ASSIGNMENT_IDENTIFICATION,           // Назначение пакета Indetification
        1,                                          // Флаги
        1, 0,                                       // Номер фрагмента
        1, 0,                                       // Кол-во фрагментов
        0, 0, 0, 0, 0, 0, 0, 0,                     // Размер тела пакета
        7, 0, 0, 0, 0, 0, 0, 0                      // Контрольная сумма тела пакета
    };

    // в transmitBuffer будет ответ на Identification по окончанию цикла
    for (uint i = 0; i < HEADER_SIZE; ++i)
    {
        if (i > 0 && i < HEADER_SIZE)
            QCOMPARE(expextedDataSize, (uint64_t)HEADER_SIZE);

        QCOMPARE(realBufSize, (uint64_t)i);
        QCOMPARE(cutDataSize, (uint64_t)0);
        QCOMPARE(error, false);

        dataRecived(dataRaw[i]);
    }

    QCOMPARE(expextedDataSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)HEADER_SIZE);
    QCOMPARE(realBufSize, (uint64_t)0);
    QCOMPARE(error, false);
}

void IOTVP_Header_Embedded_Test::test_dataRecivedRead()
{
    realBufSize = 0;
    error = false;
    expextedDataSize = 0;
    cutDataSize = 0;

    // Запрос чтения
    uint8_t dataRaw[] = {
        2,                                          // Версия протокола
        HEADER_TYPE_REQUEST,                        // Тип пакета - запрос
        HEADER_ASSIGNMENT_READ,                     // Назначение пакета Read
        HEADER_FLAGS_NONE,                          // Флаги
        1, 0,                                       // Номер фрагмента
        1, 0,                                       // Кол-во фрагментов
        21, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               21 = READ_SIZE + nameSize
        29, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма заголовка
        6,                                          // Длина имени устройства
        1,                                          // Номер канала
        0,                                          // Флаги
        0, 0, 0, 0,                                 // Размер данных
        7, 0, 0, 0, 0, 0, 0, 0,                     // Контрольная сумма
        'D', 'e', 'v', 'i', 'c', 'e'                // Имя устройства
    };

    // в transmitBuffer будет ответ на Read по окончанию цикла
    for (uint i = 0; i < HEADER_SIZE + READ_WRITE_SIZE + 6; ++i) // 6 - имя устройства
    {
        if (i > 0 && i < HEADER_SIZE)
            QCOMPARE(expextedDataSize, (uint64_t)HEADER_SIZE);

        //        if (i > HEADER_SIZE && i < HEADER_SIZE + READ_WRITE_SIZE)
        //            QCOMPARE(expextedDataSize, HEADER_SIZE + READ_WRITE_SIZE + 6);

        QCOMPARE(error, false);
        dataRecived(dataRaw[i]);
    }

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)45);
    QCOMPARE(expextedDataSize, (uint64_t)0);

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
        2,                                  // Версия протокола
        HEADER_TYPE_REQUEST,                // Тип пакета - запрос
        HEADER_ASSIGNMENT_WRITE,            // Назначение пакета Write
        HEADER_FLAGS_NONE,                  // Флаги
        1, 0,                               // Номер фрагмента
        1, 0,                               // Кол-во фрагментов
        23, 0, 0, 0, 0, 0, 0, 0,            // Размер тела пакета               23 = Wite_SIZE + nameSize + data (2)
        32, 0, 0, 0, 0, 0, 0, 0,            // Контрольная сумма тела пакета    32
        6,                                  // Длина имени устройства
        1,                                  // Номер канала
        0,                                  // Флаги
        2, 0, 0, 0,                         // Размер данных
        9, 0, 0, 0, 0, 0, 0, 0,             // Контрольная сумма
        'D', 'e', 'v', 'i', 'c', 'e',       // Имя устройства
        123, 0                              // Данные
    };

    // в transmitBuffer будет ответ на Write по окончанию цикла
    for (uint i = 0; i < HEADER_SIZE + READ_WRITE_SIZE + 6 + 2; ++i) // 6 - имя устройства, 2 - данные
    {
        if (i > 0 && i < HEADER_SIZE)
            QCOMPARE(expextedDataSize, (uint64_t)HEADER_SIZE);

        //        if (i > HEADER_SIZE && i < HEADER_SIZE + READ_WRITE_SIZE)
        //            QCOMPARE(expextedDataSize, HEADER_SIZE + READ_WRITE_SIZE + 6);

        QCOMPARE(error, false);
        dataRecived(dataRaw[i]);
    }


    QCOMPARE(error, false);
    QCOMPARE(realBufSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)47);
    QCOMPARE(expextedDataSize, (uint64_t)0);

    //    QCOMPARE(iot.readChannel[1], 123);
}

void IOTVP_Header_Embedded_Test::test_dataReciveState()
{
    {
        realBufSize = 0;
        error = false;
        expextedDataSize = 0;
        cutDataSize = 0;

        // Запрос чтения
        uint8_t dataRaw[] =  {
            2,                                          // Версия протокола
            HEADER_TYPE_REQUEST,                        // Тип пакета - запрос
            HEADER_ASSIGNMENT_STATE,                    // Назначение пакета State
            HEADER_FLAGS_NONE,                          // Флаги
            1, 0,                                       // Номер фрагмента
            1, 0,                                       // Кол-во фрагментов
            21, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               21 = STATE_SIZE + nameSize
            28, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма тела пакета    28
            6,                                          // Длина имени устройства
            0,                                          // Состояние
            0,                                          // Флаги
            0, 0, 0, 0,                                 // Размер данных
            6, 0, 0, 0, 0, 0, 0, 0,                     // Контрольная сумма
            'D', 'e', 'v', 'i', 'c', 'e'                // Имя устройства
        };

        // в transmitBuffer будет ответ на State по окончанию цикла
        for (uint i = 0; i < HEADER_SIZE + STATE_SIZE + 6; ++i) // 6 - имя устройства
        {
            if (i > 0 && i < HEADER_SIZE)
                QCOMPARE(expextedDataSize, (uint64_t)HEADER_SIZE);

            QCOMPARE(error, false);
            dataRecived(dataRaw[i]);
        }

        QCOMPARE(error, false);
        QCOMPARE(realBufSize, (uint64_t)0);
        QCOMPARE(cutDataSize, (uint64_t)45);
        QCOMPARE(expextedDataSize, (uint64_t)0);

        QCOMPARE(iot.state, State_STATE_ONLINE); //вызывается второй раз из теста write
    }

    {
        realBufSize = 0;
        error = false;
        expextedDataSize = 0;
        cutDataSize = 0;

        // Запрос чтения
        uint8_t dataRaw[] =  {
            2,                                          // Версия протокола
            HEADER_TYPE_REQUEST,                        // Тип пакета - запрос
            HEADER_ASSIGNMENT_STATE,                    // Назначение пакета State
            HEADER_FLAGS_NONE,                          // Флаги
            1, 0,                                       // Номер фрагмента
            1, 0,                                       // Кол-во фрагментов
            21, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               21 = STATE_SIZE + nameSize
            28, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма тела пакета    28
            6,                                          // Длина имени устройства
            0,                                          // Состояние
            0,                                          // Флаги
            0, 0, 0, 0,                                 // Размер данных
            6, 0, 0, 0, 0, 0, 0, 0,                     // Контрольная сумма
            'D', 'e', 'v', 'i', 'c', 'e',                // Имя устройства
            2,                                          // Версия протокола
            HEADER_TYPE_REQUEST,                        // Тип пакета - запрос
            HEADER_ASSIGNMENT_STATE,                    // Назначение пакета State
            HEADER_FLAGS_NONE,                          // Флаги
            1, 0,                                       // Номер фрагмента
            1, 0,                                       // Кол-во фрагментов
            21, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               21 = STATE_SIZE + nameSize
            28, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма тела пакета    28
            6,                                          // Длина имени устройства
            0,                                          // Состояние
            0,                                          // Флаги
            0, 0, 0, 0,                                 // Размер данных
            6, 0, 0, 0, 0, 0, 0, 0,                     // Контрольная сумма
            'D', 'e', 'v', 'i', 'c', 'e',                // Имя устройства
        };

        // в transmitBuffer будет ответ на State по окончанию цикла
        for (uint i = 0; i < (HEADER_SIZE + STATE_SIZE + 6) * 2; ++i) // 6 - имя устройства
        {
            //            if (i > 0 && i < HEADER_SIZE)
            //                QCOMPARE(expextedDataSize, HEADER_SIZE);

            QCOMPARE(error, false);
            dataRecived(dataRaw[i]);
        }

        QCOMPARE(error, false);
        QCOMPARE(realBufSize, (uint64_t)0);
        QCOMPARE(cutDataSize, (uint64_t)45);
        QCOMPARE(expextedDataSize, (uint64_t)0);

        QCOMPARE(iot.state, State_STATE_ONLINE); //вызывается второй раз из теста write
    }
}

void IOTVP_Header_Embedded_Test::test_DataTransmitPing()
{
    test_dataRecivedPing();
    // Заголовок ответ на PING
    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_PING_PONG,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = 0,
        .pkg = NULL
    };

    char outData[HEADER_SIZE];
    headerToData(&header, outData, HEADER_SIZE);

    for (uint i = 0; i < HEADER_SIZE; ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)HEADER_SIZE);
    QCOMPARE(expextedDataSize, (uint64_t)0);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitIdentification()
{
    test_dataRecivedIdentefication();

    struct Identification ident = {
        .id = iot.id,
        .descriptionSize = static_cast<uint16_t>(strlen(iot.description)),
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .numberWriteChannel = iot.numberWriteChannel,
        .numberReadChannel = iot.numberReadChannel,
        .flags = Identification_FLAGS_NONE,
        .name = iot.name,
        .description = iot.description,
        .writeChannelType = iot.writeChannelType,
        .readChannelType = iot.readChannelType
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = identificationSize(&ident),
        .pkg = NULL
    };

    char outData[headerSize(&header)];
    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)HEADER_SIZE); // осталось от test_dataRecivedIdentefication
    QCOMPARE(expextedDataSize, (uint64_t)0);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitRead()
{
    // test_dataRecivedRead - кладёт в буфер данные для отправки на полученый пакет в тесте.
    // В отправляемых даных размер больше на .data
    test_dataRecivedRead();

    struct Read_Write readWrite = {
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .channelNumber = 1,
        .flags = ReadWrite_FLAGS_NONE,
        .dataSize = iot.readChannel[1].dataSize,
        .name = iot.name,
        .data = iot.readChannel[1].data
    };

    struct Header header = {
        .version = 2,

        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_READ,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite
    };

    char outData[headerSize(&header)];
    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)(HEADER_SIZE + READ_WRITE_SIZE + 6) /*Имя устройства*/); // осталось от test_dataTransmitRead
    QCOMPARE(expextedDataSize, (uint64_t)0);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitWrite()
{
    // test_dataRecivedWrite - кладёт в буфер данные для отправки на полученый пакет в тесте.
    test_dataRecivedWrite();

    struct Read_Write readWrite = {
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .channelNumber = 1,
        .flags = ReadWrite_FLAGS_NONE,
        .dataSize = iot.readChannel[1].dataSize,
        .name = iot.name,
        .data = iot.readChannel[1].data
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_WRITE,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite,
    };

    char outData[headerSize(&header)];
    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)(HEADER_SIZE + READ_WRITE_SIZE + 6 + 2)); // осталось от test_dataRecivedWrite 45 = HEADER_SIZE + Wite_SIZE + nameSize + data (2)
    QCOMPARE(expextedDataSize, (uint64_t)0);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitState()
{
    // test_dataReciveState - кладёт в буфер данные для отправки на полученый пакет в тесте.
    test_dataReciveState();

    struct State state = {
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .state = iot.state,
        .flags = STATE_FLAGS_NONE,
        .dataSize = 0,
        .name = iot.name,
        .data = NULL
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_STATE,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = stateSize(&state),
        .pkg = &state
    };

    char outData[headerSize(&header)];
    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, (uint64_t)0);
    QCOMPARE(cutDataSize, (uint64_t)(HEADER_SIZE + STATE_SIZE + 6 /*Имя устройства*/)); // осталось от test_dataTransmitState
    QCOMPARE(expextedDataSize, (uint64_t)0);
}

void IOTVP_Header_Embedded_Test::test_dataTransmitWriteFragmet()
{
    struct Header* header = createPkgs((uint8_t*)recivedBuffer, realBufSize, &error, &expextedDataSize, &cutDataSize);
    struct Read_Write *ptrReadWrite = ((struct Read_Write *)header->pkg);

    QFile file("test_dataTransmitWriteFragmets_2.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    file.write(ptrReadWrite->data, ptrReadWrite->dataSize);
    file.close();
}

void IOTVP_Header_Embedded_Test::test_dataTransmitWriteFragmets()
{
    {
        QFile file("test_dataTransmitWriteFragmets.txt");
        file.open(QIODevice::ReadWrite);
        QFile file2("test_dataTransmitWriteFragmets_2.txt");
        file2.open(QIODevice::ReadWrite);

        file.remove();
        file2.remove();
    }

    QFile file("test_dataTransmitWriteFragmets.txt");
    file.open(QIODevice::WriteOnly);

    for (int i = 0; i < 1'000; ++i)
        file.write("Hello world!!!\n");
    file.close();

    uint64_t sizeToSend = file.size();
    uint64_t countPkg = pkgCount(sizeToSend, BUFSIZ, HEADER_SIZE + READ_WRITE_SIZE + strlen(iot.name));

    file.open(QIODevice::ReadOnly);
    for (uint64_t i = 0; i < countPkg; ++i)
    {
        auto dataRead = file.read(BUFSIZ - (HEADER_SIZE + READ_WRITE_SIZE + strlen(iot.name)));

        struct Read_Write readWrite = {
            .nameSize = static_cast<uint8_t>(strlen(iot.name)),
            .channelNumber = 1,
            .flags = ReadWrite_FLAGS_NONE,
            .dataSize = static_cast<uint64_t>(dataRead.size()),
            .name = iot.name,
            .data = dataRead.data()
        };

        struct Header header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_READ,
            .flags = HEADER_FLAGS_NONE,
            .fragment = static_cast<uint16_t>(i + 1),
            .fragments = static_cast<uint16_t>(countPkg),
            .dataSize = readWriteSize(&readWrite),
            .pkg = &readWrite
        };

        realBufSize = headerToData(&header, recivedBuffer, BUFSIZ);
        test_dataTransmitWriteFragmet();
    }
    file.close();

    {
        QFile file("test_dataTransmitWriteFragmets.txt");
        file.open(QIODevice::ReadOnly);
        QFile file2("test_dataTransmitWriteFragmets_2.txt");
        file2.open(QIODevice::ReadOnly);

        QCOMPARE(file.size(), file2.size());
        QCOMPARE(file.readAll(), file2.readAll());
    }

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, (uint64_t)6898);
    QCOMPARE(cutDataSize, (uint64_t)6898);
    QCOMPARE(expextedDataSize, (uint64_t)0);
}

void IOTVP_Header_Embedded_Test::test_pkgCount()
{
    auto result = pkgCount(1024, 1024, 8);
    QCOMPARE(result, (uint64_t)2);

    result = pkgCount(1024, 1024, 0);
    QCOMPARE(result, (uint64_t)1);

    result = pkgCount(1020, 1024, 0);
    QCOMPARE(result, (uint64_t)1);

    result = pkgCount(1020, 1024, 4);
    QCOMPARE(result, (uint64_t)1);

    result = pkgCount(1024, 512, 10);
    QCOMPARE(result, (uint64_t)3);
}

void IOTVP_Header_Embedded_Test::test_dataPart()
{
    QFile file("test_dataTransmitWriteFragmets.txt");
    file.open(QIODevice::ReadOnly);

    uint8_t readType = {DATA_TYPE_STRING};

    struct IOTV_Server_embedded iot = {
        .id = 1,

        .numberReadChannel = 1,
        .numberWriteChannel = 0,

        .state = 1,

        .nameSize = static_cast<uint8_t>(strlen(name)),
        .descriptionSize = static_cast<uint16_t>(strlen(description)),

        .readChannel = NULL,
        .readChannelType = &readType,
        .writeChannelType = NULL,

        .name = (char *)name,
        .description = (char *)description,
    };

    iot.readChannel = (struct RawEmbedded*)malloc(sizeof(struct RawEmbedded));
    iot.readChannel[0].dataSize = file.size();

    iot.readChannel[0].data = file.readAll().data();

    char *data = iot.readChannel[0].data;

    auto pkgsCount = pkgCount(file.size(), 1024, 0);
    for (uint i = 0; i < pkgsCount - 1; ++i)
    {
        auto result = dataPart(data, i, 1024, &iot, 0);
        QCOMPARE(result, (uint64_t)1024);
    }

    auto result = dataPart(data, pkgsCount - 1, 1024, &iot, 0);
    QCOMPARE(result, (uint64_t)file.size() - 1024 * (pkgsCount - 1));

    free(iot.readChannel);
}

QTEST_APPLESS_MAIN(IOTVP_Header_Embedded_Test)

#include "tst_iotvp_header_embedded_test.moc"

