#include <QtTest>
#include <QtEndian>
#include <QtCore>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

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
        .name = "Device",
        .description = "Description",
        .numberReadChannel = 3,
        .readChannel = NULL,
        .readChannelType = readType,
        .numberWriteChannel = 2,
        .writeChannelType = writeType,
        .state = 1,
        .nameSize = static_cast<uint8_t>(strlen(iot.name)),
        .descriptionSize = static_cast<uint16_t>(strlen(iot.description))
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
    void test_dataReciveState();

    void test_DataTransmitPing();
    void test_dataTransmitIdentification();
    void test_dataTransmitRead();
    void test_dataTransmitWrite();
    void test_dataTransmitState();

    void timeCompare();
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
            /* uint64_t size = */responseReadData(transmitBuffer, BUFSIZ, &iot, header);
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
        0, 0, 0, 0, 0, 0, 0, 0,                      // Размер тела пакета
        9, 0, 0, 0, 0, 0, 0, 0                       // Контрольная сумма тела пакета
    };

    const struct Header *header = createHeader(dataRaw, HEADER_SIZE, &error, &expextedDataSize, &cutDataSize);
    QCOMPARE(error, false);
    QCOMPARE(expextedDataSize, 0);
    QCOMPARE(cutDataSize, HEADER_SIZE);
    QCOMPARE(header->type, HEADER_TYPE_REQUEST);
    QCOMPARE(header->assignment, HEADER_ASSIGNMENT_PING_PONG);
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
        0, 0, 0, 0, 0, 0, 0, 0,                      // Размер тела пакета
        9, 0, 0, 0, 0, 0, 0, 0                       // Контрольная сумма тела пакета
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
        HEADER_TYPE_REQUEST             ,   // Тип пакета - запрос
        HEADER_ASSIGNMENT_IDENTIFICATION,   // Назначение пакета Indetification
        HEADER_FLAGS_ERROR,                 // Флаги
        0, 0, 0, 0, 0, 0, 0, 0,                     // Размер тела пакета
        3, 1, 0, 0, 0, 0, 0, 0                      // Контрольная сумма тела пакета  - 259
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
        HEADER_TYPE_REQUEST,                // Тип пакета - запрос
        HEADER_ASSIGNMENT_READ,             // Назначение пакета Read
        HEADER_FLAGS_NONE,                  // Флаги
        21, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               21 = READ_SIZE + nameSize
        27, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма тела пакета    27
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
        HEADER_TYPE_REQUEST,                // Тип пакета - запрос
        HEADER_ASSIGNMENT_WRITE,            // Назначение пакета Write
        HEADER_FLAGS_NONE,                  // Флаги
        25, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               25 = Wite_SIZE + nameSize + data (4)
        32, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма тела пакета    32
        6,                                          // Длина имени устройства
        1,                                          // Номер канала
        0,                                          // Флаги
        4, 0, 0, 0,                                 // Размер данных
        11, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма
        'D', 'e', 'v', 'i', 'c', 'e',               // Имя устройства
        123, 0, 0, 0                                // Данные
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
            HEADER_TYPE_REQUEST,                // Тип пакета - запрос
            HEADER_ASSIGNMENT_STATE,            // Назначение пакета State
            HEADER_FLAGS_NONE,                  // Флаги
            21, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               21 = STATE_SIZE + nameSize
            26, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма тела пакета    27
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
                QCOMPARE(expextedDataSize, HEADER_SIZE);

            QCOMPARE(error, false);
            dataRecived(dataRaw[i]);
        }

        QCOMPARE(error, false);
        QCOMPARE(realBufSize, 0);
        QCOMPARE(cutDataSize, 41);
        QCOMPARE(expextedDataSize, 0);

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
            HEADER_TYPE_REQUEST,                // Тип пакета - запрос
            HEADER_ASSIGNMENT_STATE,            // Назначение пакета State
            HEADER_FLAGS_NONE,                  // Флаги
            21, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               21 = STATE_SIZE + nameSize
            26, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма тела пакета    27
            6,                                          // Длина имени устройства
            0,                                          // Состояние
            0,                                          // Флаги
            0, 0, 0, 0,                                 // Размер данных
            6, 0, 0, 0, 0, 0, 0, 0,                     // Контрольная сумма
            'D', 'e', 'v', 'i', 'c', 'e',                // Имя устройства
            2,                                          // Версия протокола
            HEADER_TYPE_REQUEST,                // Тип пакета - запрос
            HEADER_ASSIGNMENT_STATE,            // Назначение пакета State
            HEADER_FLAGS_NONE,                  // Флаги
            21, 0, 0, 0, 0, 0, 0, 0,                    // Размер тела пакета               21 = STATE_SIZE + nameSize
            26, 0, 0, 0, 0, 0, 0, 0,                    // Контрольная сумма тела пакета    27
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
        QCOMPARE(realBufSize, 0);
        QCOMPARE(cutDataSize, 41);
        QCOMPARE(expextedDataSize, 0);

        QCOMPARE(iot.state, State_STATE_ONLINE); //вызывается второй раз из теста write
    }
}

void IOTVP_Header_Embedded_Test::test_DataTransmitPing()
{
    test_dataRecivedPing();
    // Заголовок ответ на PING
    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_PING_PONG,
                .flags = HEADER_FLAGS_NONE,
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
        .flags = Identification_FLAGS_NONE,
                .id = iot.id,
                .nameSize = static_cast<uint8_t>(strlen(iot.name)),
                .descriptionSize = static_cast<uint16_t>(strlen(iot.description)),
                .numberWriteChannel = iot.numberWriteChannel,
                .numberReadChannel = iot.numberReadChannel,
                .name = iot.name,
                .description = iot.description,
                .writeChannelType = iot.writeChannelType,
                .readChannelType = iot.readChannelType
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
                .flags = HEADER_FLAGS_NONE,
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
        .flags = ReadWrite_FLAGS_NONE,
                .nameSize = static_cast<uint8_t>(strlen(iot.name)),
                .channelNumber = 1,
                .dataSize = iot.readChannel[1].dataSize,
                .name = iot.name,
                .data = iot.readChannel[1].data
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_READ,
                .flags = HEADER_FLAGS_NONE,
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
        .flags = ReadWrite_FLAGS_NONE,
                .nameSize = static_cast<uint8_t>(strlen(iot.name)),
                .channelNumber = 1,
                .dataSize = iot.readChannel[1].dataSize,
                .name = iot.name,
                .data = iot.readChannel[1].data
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_WRITE,
                .flags = HEADER_FLAGS_NONE,
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

void IOTVP_Header_Embedded_Test::test_dataTransmitState()
{
    // test_dataReciveState - кладёт в буфер данные для отправки на полученый пакет в тесте.
    test_dataReciveState();

    struct State state = {
        .flags = STATE_FLAGS_NONE,
                .state = static_cast<State_STATE>(iot.state),
                .nameSize = static_cast<uint8_t>(strlen(iot.name)),
                .dataSize = 0,
                .name = iot.name,
                .data = NULL
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_STATE,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
                .dataSize = stateSize(&state),
                .identification = NULL,
                .readWrite = NULL,
                .state = &state
    };

    char outData[headerSize(&header)];
    headerToData(&header, outData, headerSize(&header));

    for (uint i = 0; i < headerSize(&header); ++i)
        QCOMPARE(outData[i], transmitBuffer[i]);

    QCOMPARE(error, false);
    QCOMPARE(realBufSize, 0);
    QCOMPARE(cutDataSize, HEADER_SIZE + STATE_SIZE + 6 /*Имя устройства*/); // осталось от test_dataTransmitState
    QCOMPARE(expextedDataSize, 0);
}

void IOTVP_Header_Embedded_Test::timeCompare()
{
    // Сравнение скороси выполнения разных версий кода
//    // Запрос записи
//    uint8_t dataRaw[] =  {
//        2,                                          // Версия протокола
//        HEADER_TYPE_REQUEST,                // Тип пакета - запрос
//        HEADER_ASSIGNMENT_WRITE,            // Назначение пакета Write
//        HEADER_FLAGS_NONE,                  // Флаги
//        0, 0, 0, 0, 0, 0, 0, 25,                    // Размер тела пакета               25 = Wite_SIZE + nameSize + data (4)
//        0, 0, 0, 0, 0, 0, 0, 32,                    // Контрольная сумма тела пакета    32
//        6,                                          // Длина имени устройства
//        1,                                          // Номер канала
//        0,                                          // Флаги
//        0, 0, 0, 4,                                 // Размер данных
//        0, 0, 0, 0, 0, 0, 0, 11,                    // Контрольная сумма
//        'D', 'e', 'v', 'i', 'c', 'e',               // Имя устройства
//        0, 0, 0, 123                                // Данные
//    };

//    auto start = std::chrono::system_clock::now();

//    realBufSize = 0;
//    error = false;
//    expextedDataSize = 0;
//    cutDataSize = 0;

//    for (int x = 0; x < 100'000; ++x)
//    {
//        // в transmitBuffer будет ответ на Write по окончанию цикла
//        for (uint i = 0; i < HEADER_SIZE + READ_WRITE_SIZE + 6 + 4; ++i) // 6 - имя устройства, 4 - данные
//            dataRecived(dataRaw[i]);
//    }

//    qDebug() << "IOTVP Embedded protocol - " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count();

//    QByteArray rawData(reinterpret_cast<char*>(dataRaw), HEADER_SIZE + READ_WRITE_SIZE + 6 + 4);

//    start = std::chrono::system_clock::now();

//    for (int x = 0; x < 100'000; ++x)
//    {
//        IOTVP_Creator creator(rawData);
//        creator.createPkgs();
//        auto header = creator.takeHeader();
//        //        auto transmit = header->toData();
//    }

//    qDebug() << "IOTVP protocol - " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count();
}

QTEST_APPLESS_MAIN(IOTVP_Header_Embedded_Test)

#include "tst_iotvp_header_embedded_test.moc"

