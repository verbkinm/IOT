#include "IOTV_SH.h"
#include "identification.h"
#include "read_write.h"
#include "state.h"
#include "tech.h"
#include "log_data.h"
#include "iotv_types.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static uint64_t responceReadWritePkgCount(uint64_t dataOutSize, const struct IOTV_Server_embedded *iot, const struct Header *header);

uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, uint8_t ident_flags)
{
    if (outData == NULL)
        return 0;

    if (iot == NULL)
    {
        struct Header header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
            .flags = HEADER_FLAGS_NONE,
            .fragment = 1,
            .fragments = 1,
            .dataSize = 0,
            .pkg = NULL
        };

        return headerToData(&header, outData, dataSize);
    }

    struct Identification ident = {
        .id = iot->id,
        .descriptionSize = iot->descriptionSize,
        .nameSize = iot->nameSize,
        .numberWriteChannel = iot->numberWriteChannel,
        .numberReadChannel = iot->numberReadChannel,
        .flags = ident_flags,
        .name = iot->name,
        .description = iot->description,
        .writeChannelType = iot->writeChannelType,
        .readChannelType = iot->readChannelType
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = identificationSize(&ident),
        .pkg = &ident
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t responsePingData(char *outData, uint64_t dataSize)
{
    if (outData == NULL)
        return 0;

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

    return headerToData(&header, outData, dataSize);
}

uint64_t responseReadData(char *outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, const struct Header *head, uint64_t (*writeFunc)(char *, uint64_t, void *obj), void *obj)
{
    struct Read_Write *rwPkg = ((struct Read_Write *)head->pkg);
    uint64_t pkgsCount;
    uint64_t totalSendByte = 0;

    if (rwPkg->flags == ReadWrite_FLAGS_IGNORE_CH)
    {
        for (int i = 0; i < iot->numberReadChannel; ++i)
        {
            rwPkg->flags = ReadWrite_FLAGS_NONE;
            rwPkg->channelNumber = i;

            totalSendByte += responseReadData(outData, dataSize, iot, head, writeFunc, obj);
        }
        return totalSendByte;
    }

    if (rwPkg->flags != ReadWrite_FLAGS_OPEN_STREAM
        && (iot->readChannelType[rwPkg->channelNumber] == DATA_TYPE_RAW
            || iot->readChannelType[rwPkg->channelNumber] == DATA_TYPE_STRING
            || iot->readChannelType[rwPkg->channelNumber] == DATA_TYPE_NONE))
    {
        return 0;
    }

    pkgsCount = responceReadWritePkgCount(dataSize, iot, head);
    for (uint16_t i = 0; i < pkgsCount; ++i)
    {
        char *it = NULL;
        uint64_t dataReadSize = dataPart(&it, i, dataSize - (HEADER_SIZE + READ_WRITE_SIZE + iot->nameSize), iot, rwPkg->channelNumber);

        struct Read_Write readWrite = {
            .dataSize = dataReadSize,
            .nameSize = iot->nameSize,
            .channelNumber = rwPkg->channelNumber,
            .flags = rwPkg->flags,
            .name = iot->name,
            .data = it
        };

        struct Header header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_READ,
            .flags = HEADER_FLAGS_NONE,
            .fragment = (uint16_t)(i + 1),
            .fragments = (uint16_t)(pkgsCount),
            .dataSize = readWriteSize(&readWrite),
            .pkg = &readWrite
        };

        uint64_t resultSize = headerToData(&header, outData, dataSize);
        totalSendByte += writeFunc(outData, resultSize, obj);
    }

    return totalSendByte;
}

uint64_t responseWriteData(char *outData, uint64_t dataSize, struct IOTV_Server_embedded *iot, const struct Header *head)
{
    if (head == NULL || outData == NULL || iot == NULL || head->pkg == NULL)
        return 0;

    struct Read_Write *ptrReadWrite = ((struct Read_Write *)head->pkg);

    if (iot->readChannel == NULL
        || iot->numberWriteChannel <= ptrReadWrite->channelNumber
        || iot->numberReadChannel <= ptrReadWrite->channelNumber
        || iot->readChannel[ptrReadWrite->channelNumber].data == NULL)
        return 0;

    if (ptrReadWrite->dataSize > 0)
    {
        if (iot->readChannel[ptrReadWrite->channelNumber].dataSize != ptrReadWrite->dataSize) //!!!
            return 0;

        memcpy(iot->readChannel[ptrReadWrite->channelNumber].data, ptrReadWrite->data, iot->readChannel[ptrReadWrite->channelNumber].dataSize);
    }

    struct Read_Write readWrite = {
        .dataSize = iot->readChannel[ptrReadWrite->channelNumber].dataSize,
        .nameSize = iot->nameSize,
        .channelNumber = ptrReadWrite->channelNumber,
        .flags = ReadWrite_FLAGS_NONE,
        .name = iot->name,
        .data = iot->readChannel[ptrReadWrite->channelNumber].data
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_WRITE,
        .flags = HEADER_FLAGS_NONE,
        .fragment = head->fragment,
        .fragments = head->fragments,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite,
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t responseStateData(char *outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot)
{
    if (outData == NULL || iot == NULL)
        return 0;

    struct State state = {
        .nameSize = iot->nameSize,
        .state = iot->state,
        .flags = STATE_FLAGS_NONE,
        .dataSize = 0,
        .name = iot->name,
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

    return headerToData(&header, outData, dataSize);
}

uint64_t queryIdentificationData(char *outData, uint64_t dataSize)
{
    if (outData == NULL)
        return 0;

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = 0,
        .pkg = NULL
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t queryPingData(char *outData, uint64_t dataSize)
{
    if (outData == NULL)
        return 0;

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_PING_PONG,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = 0,
        .pkg = NULL
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t queryWriteData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, const char *dataToWrite, uint32_t dataWriteSize)
{
    if (outData == NULL || dataToWrite == NULL)
        return 0;

    struct Read_Write readWrite = {
        .dataSize = dataWriteSize,
        .nameSize = (uint8_t)strlen(name),
        .channelNumber = channelNumber,
        .flags =ReadWrite_FLAGS_NONE,
        .name = (char *)name,
        .data = (char *)dataToWrite
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_WRITE,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite,
    };

    return headerToData(&header, outData, outDataSize);
}

uint64_t queryReadData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, uint8_t flags)
{
    if (outData == NULL)
        return 0;

    struct Read_Write readWrite = {
        .dataSize = 0,
        .nameSize = (uint8_t)strlen(name),
        .channelNumber = channelNumber,
        .flags = flags,
        .name = (char *)name,
        .data = NULL
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_READ,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite,
    };

    return headerToData(&header, outData, outDataSize);
}

uint64_t queryStateData(char *outData, uint64_t outDataSize, const char *name)
{
    if (outData == NULL)
        return 0;

    struct State state = {
        .nameSize = (uint8_t)strlen(name),
        .state = State_STATE_OFFLINE,
        .flags = STATE_FLAGS_NONE,
        .dataSize = 0,
        .name = (char *)name,
        .data = NULL
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_STATE,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = stateSize(&state),
        .pkg = &state
    };

    return headerToData(&header, outData, outDataSize);
}

struct IOTV_Server_embedded *createIotFromHeaderIdentification(const struct Header *header)
{
    if (header == NULL || header->pkg == NULL)
        return NULL;

    struct Identification *ptrIdentification = ((struct Identification *)header->pkg);

    uint8_t nameSize = ptrIdentification->nameSize;
    uint16_t descriptionSize = ptrIdentification->descriptionSize;

    uint8_t numberWriteChannel = ptrIdentification->numberWriteChannel;
    uint8_t numberReadChannel = ptrIdentification->numberReadChannel;

    struct IOTV_Server_embedded *ptrIot = (struct IOTV_Server_embedded *)malloc(sizeof(struct IOTV_Server_embedded));
    if (ptrIot == NULL)
        return NULL;

    struct IOTV_Server_embedded iot = {
        .id = ptrIdentification->id,
        .numberReadChannel = numberReadChannel,
        .numberWriteChannel = numberWriteChannel,
        .state = false,
        .nameSize = nameSize,
        .descriptionSize = descriptionSize,
        .readChannel = (numberReadChannel > 0) ? (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * numberReadChannel) : NULL,
        .readChannelType = (numberReadChannel > 0) ? (uint8_t *)malloc(numberReadChannel) : NULL,
        .writeChannelType = (numberWriteChannel > 0) ? (uint8_t *)malloc(numberWriteChannel) : NULL,
        .name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL,
        .description = (descriptionSize > 0) ? (char *)malloc(descriptionSize) : NULL
    };

    if (nameSize > 0)
        memcpy((void *)iot.name, ptrIdentification->name, nameSize);
    if (descriptionSize > 0)
        memcpy((void *)iot.description, ptrIdentification->description, descriptionSize);

    bool mallocFlag = false;
    for (int i = 0; i < numberReadChannel; ++i)
    {
        iot.readChannel[i].dataSize = dataSizeonDataType(ptrIdentification->readChannelType[i]);
        iot.readChannel[i].data = (char *)malloc(iot.readChannel[i].dataSize);
        if (iot.readChannel[i].dataSize > 0 && iot.readChannel[i].data == NULL)
        {
            // не удалось выделить память!
            mallocFlag = true;
            break;
        }
        iot.readChannelType[i] = ptrIdentification->readChannelType[i];
    }

    for (int i = 0; i < numberWriteChannel; ++i)
        iot.writeChannelType[i] = ptrIdentification->writeChannelType[i];

    memcpy((void *)ptrIot, &iot, sizeof(struct IOTV_Server_embedded));

    // Проверка malloc!
    if (mallocFlag
        ||(nameSize > 0 && ptrIot->name == NULL)
        || (descriptionSize > 0 && ptrIot->description == NULL)
        || (numberReadChannel > 0 && (ptrIot->readChannel == NULL || ptrIot->readChannelType == NULL))
        || (numberWriteChannel > 0 && ptrIot->writeChannelType == NULL))
    {
        clearIOTV_Server(ptrIot);
    }

    return ptrIot;
}


uint64_t responseTech(char *outData, uint64_t outDataSize, const char *inData, uint64_t inDataSize, const Header *head)
{
    if (outData == NULL || inData == NULL || head == NULL || head->pkg == NULL)
        return 0;

    const struct Tech *pkg = (const struct Tech *)(head->pkg);

    struct Tech tech = {
        .flags = Tech_FLAGS_NONE,
        .type = pkg->type,
        .dataSize = inDataSize,
        .data = (uint8_t *)inData
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_TECH,
        .flags = HEADER_FLAGS_NONE,
        .fragment = head->fragment,
        .fragments = head->fragments,
        .dataSize = techSize(&tech),
        .pkg = &tech
    };

    return headerToData(&header, outData, outDataSize);
}

uint64_t responseLogData(const char *fileName, char *outData, uint64_t outDataSize, const struct Log_Data *pkg, uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj)
{
    if (fileName == NULL || outData == NULL || outDataSize == 0 || pkg == NULL)
        return 0;

    FILE *file = fopen(fileName, "r");
    if (file == NULL)
        return  0;

    int64_t lastTime = 0, logLine = 0;
    uint64_t totalSendByte = 0;

    while (!feof(file))
    {
        ++logLine;

        char value[BUFSIZ] = {0};
        char rw;
        uint64_t ms;
        int ch, year, month, day, hour, minut, second;

        char line [BUFSIZ] = {0};
        if (fgets(line, sizeof(line), file) == NULL)
            break;

        if (strlen(line) < 31) // 31 - yyyy.MM.dd hh:mm:ss:zzz - R:0=значение
        {
            fprintf(stderr, "responseLogData, erro in %ld line\n", logLine);
            continue;
        }

        sscanf(line, "%d%*c%d%*c%d%*c"
                     "%d%*c%d%*c%d%*c%ld%*c%*c%*c"
                     "%c%*c%d%*c%s",
               &year, &month, &day,
               &hour, &minut, &second,
               &ms, &rw, &ch, value);

        tm tm{};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minut;
        tm.tm_sec = second;

        ms = mktime(&tm) * 1000 + ms;

        // Фильтр по интервалу даты/времени
        if (ms < pkg->startInterval)
            continue;
        if (ms > pkg->endInterval)
            break;

        // Фильтр по интервалу между записями в лог файле
        if ((ms - lastTime) < pkg->interval)
            continue;

        // Фильтр номера канала
        if (pkg->channelNumber != ch)
            continue;

        // Фильтр напрвавления чтение/запись
        if ((pkg->flags == LOG_DATA_R && rw != 'R') || (pkg->flags == LOG_DATA_W && rw != 'W'))
            continue;

        lastTime = ms;

        uint16_t valueSize = strlen(value);
        uint32_t sendDataSize = 8 + 2 + valueSize;
        char data[sendDataSize];

        memcpy(data, &ms, sizeof(ms));
        memcpy(&data[8], &valueSize, sizeof(valueSize));
        memcpy(&data[10], value, valueSize);

        struct Log_Data logData;
        memcpy(&logData, pkg, sizeof(logData));
        logData.dataSize = sendDataSize;
        logData.data = data;

        struct Header header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_LOG_DATA,
            .flags = HEADER_FLAGS_NONE,
            .fragment = 1,
            .fragments = 1,
            .dataSize = logDataSize(&logData),
            .pkg = &logData
        };

        uint64_t resultSize = headerToData(&header, outData, outDataSize);
        totalSendByte += writeFunc(outData, resultSize, obj);
    }

    // Пакет с нулевым фрагментом данных - завершающий пакет!
    struct Log_Data logData;
    memcpy(&logData, pkg, sizeof(logData));
    logData.dataSize = 0;
    logData.data = NULL;

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_LOG_DATA,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = logDataSize(&logData),
        .pkg = &logData
    };

    printf("channel %d - stop fragment\n", logData.channelNumber);

    fflush(stdout);

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    totalSendByte += writeFunc(outData, resultSize, obj);

    fclose(file);
    return totalSendByte;
}

uint64_t queryTech(char *outData, uint64_t dataSize, const char *inData, uint64_t inDataSize, uint8_t type)
{
    if (outData == NULL || (inData == NULL && inDataSize > 0))
        return 0;

    struct Tech tech = {
        .flags = Tech_FLAGS_NONE,
        .type = type,
        .dataSize = inDataSize,
        .data = (uint8_t *)inData
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_TECH,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = techSize(&tech),
        .pkg = &tech
    };

    return headerToData(&header, outData, dataSize);
}

static uint64_t responceReadWritePkgCount(uint64_t dataOutSize, const struct IOTV_Server_embedded *iot, const struct Header *header)
{
    if (header == NULL || header->pkg == NULL || iot->readChannel == NULL || iot == NULL)
        return 0;

    struct Read_Write *ptrReadWrite = ((struct Read_Write *)header->pkg);
    if (iot->numberReadChannel <= ptrReadWrite->channelNumber)
        return 0;

    uint64_t sendDataSize = iot->readChannel[ptrReadWrite->channelNumber].dataSize;
    return pkgCount(sendDataSize, dataOutSize, HEADER_SIZE + READ_WRITE_SIZE + iot->nameSize);
}

uint64_t queryLogData(char *outData, uint64_t outDataSize, const char *name, uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, uint8_t flags)
{
    if (outData == NULL)
        return 0;

    struct Log_Data logData = {
        .startInterval = startInterval,
        .endInterval = endInterval,
        .interval = interval,
        .dataSize = 0,
        .nameSize = (uint8_t)strlen(name),
        .channelNumber = channelNumber,
        .flags = flags,
        .name = (char *)name,
        .data = NULL
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_LOG_DATA,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = logDataSize(&logData),
        .pkg = &logData,
    };

    return headerToData(&header, outData, outDataSize);
}
