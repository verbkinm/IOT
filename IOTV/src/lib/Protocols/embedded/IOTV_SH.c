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

static uint64_t responceReadWritePkgCount(uint64_t dataOutSize, const iotv_obj_t *iot, const header_t *header);

uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const iotv_obj_t *iot, identification_flag_t ident_flags)
{
    if (outData == NULL)
        return 0;

    if (iot == NULL)
    {
        header_t header = {
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

    header_t header = {
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

    header_t header = {
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

uint64_t responseReadData(char *outData, uint64_t dataSize, const iotv_obj_t *iot, const header_t *head, uint64_t (*writeFunc)(char *, uint64_t, void *obj), void *obj, readwrite_flag_t rw_flags, header_flag_t header_flags)
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

            totalSendByte += responseReadData(outData, dataSize, iot, head, writeFunc, obj, 0, 0);
        }
        return totalSendByte;
    }

    if (rwPkg->flags != ReadWrite_FLAGS_OPEN_STREAM
        && (iot->readChannelType[rwPkg->channelNumber] == DATA_TYPE_RAW
            //					|| iot->readChannelType[rwPkg->channelNumber] == DATA_TYPE_STRING
            || iot->readChannelType[rwPkg->channelNumber] == DATA_TYPE_NONE))
    {
        return 0;
    }

    //    if (rwPkg->channelNumber == 40)
    //    {
    //        printf("responseReadData ch 40, pkgsCount = %d\n", (int)pkgsCount);
    //    }

    pkgsCount = responceReadWritePkgCount(dataSize, iot, head);

    //    if (rwPkg->channelNumber == 40)
    //    {
    //        printf("responseReadData ch 40, pkgsCount = %d\n", (int)pkgsCount);
    //    }

    for (uint16_t i = 0; i < pkgsCount; ++i)
    {
        char *it = NULL;
        uint64_t dataReadSize = dataPart(&it, i, dataSize - (HEADER_SIZE + READ_WRITE_SIZE + iot->nameSize), iot, rwPkg->channelNumber);

        //        if (rwPkg->channelNumber == 40)
        //        {
        //            printf("responseReadData ch 40, dataReadSize = %d\n", (int)dataReadSize);
        //            if (it == NULL)
        //                printf("it = NULL\n");
        //        }


        struct Read_Write readWrite = {
            .dataSize = dataReadSize,
            .nameSize = iot->nameSize,
            .channelNumber = rwPkg->channelNumber,
            .flags = rwPkg->flags,
            .name = iot->name,
            .data = it
        };

        //		if (rwPkg->channelNumber == 15)
        //		{
        //			printf("responseReadData ch 15, dataSize = %d\n", (int)dataReadSize);
        //			if (readWrite.data != NULL)
        //			{
        //				for (int i = 0; i < dataReadSize; ++i)
        //					printf("%c", readWrite.data[i]);
        //				printf("\n");
        //			}
        //		}

        header_t header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_READ,
            .flags = header_flags,
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

uint64_t responseWriteData(char *outData, uint64_t dataSize, iotv_obj_t *iot, const header_t *head, readwrite_flag_t rw_flags, header_flag_t header_flags)
{
    if (head == NULL || outData == NULL || iot == NULL || head->pkg == NULL)
        return 0;

    struct Read_Write *ptrReadWrite = ((struct Read_Write *)head->pkg);

    if (iot->readChannel == NULL
        || iot->numberWriteChannel <= ptrReadWrite->channelNumber
        || iot->numberReadChannel <= ptrReadWrite->channelNumber)
        return 0;


    if (ptrReadWrite->dataSize > 0)
    {
        if (iot->readChannel[ptrReadWrite->channelNumber].data != NULL
            && iot->readChannel[ptrReadWrite->channelNumber].dataSize == ptrReadWrite->dataSize)
            memcpy(iot->readChannel[ptrReadWrite->channelNumber].data, ptrReadWrite->data, iot->readChannel[ptrReadWrite->channelNumber].dataSize);
    }

    struct Read_Write readWrite = {
        .dataSize = iot->readChannel[ptrReadWrite->channelNumber].dataSize,
        .nameSize = iot->nameSize,
        .channelNumber = ptrReadWrite->channelNumber,
        .flags = rw_flags,
        .name = iot->name,
        .data = iot->readChannel[ptrReadWrite->channelNumber].data
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_WRITE,
        .flags = header_flags,
        .fragment = head->fragment,
        .fragments = head->fragments,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite,
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t responseStateData(char *outData, uint64_t dataSize, const iotv_obj_t *iot)
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

    header_t header = {
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

    header_t header = {
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

    header_t header = {
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

    header_t header = {
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

    header_t header = {
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

    header_t header = {
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

iotv_obj_t *createIotFromHeaderIdentification(const header_t *header)
{
    if (header == NULL || header->pkg == NULL)
        return NULL;

    struct Identification *ptrIdentification = ((struct Identification *)header->pkg);

    uint8_t nameSize = ptrIdentification->nameSize;
    uint16_t descriptionSize = ptrIdentification->descriptionSize;

    uint8_t numberWriteChannel = ptrIdentification->numberWriteChannel;
    uint8_t numberReadChannel = ptrIdentification->numberReadChannel;

    iotv_obj_t *ptrIot = (iotv_obj_t *)malloc(sizeof(iotv_obj_t));
    if (ptrIot == NULL)
        return NULL;

    iotv_obj_t iot = {
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

    memcpy((void *)ptrIot, &iot, sizeof(iotv_obj_t));

    // Проверка malloc!
    if (mallocFlag
        ||(nameSize > 0 && ptrIot->name == NULL)
        || (descriptionSize > 0 && ptrIot->description == NULL)
        || (numberReadChannel > 0 && (ptrIot->readChannel == NULL || ptrIot->readChannelType == NULL))
        || (numberWriteChannel > 0 && ptrIot->writeChannelType == NULL))
    {
        clear_iotv_obj(ptrIot);
    }

    return ptrIot;
}


uint64_t responseTech(char *outData, uint64_t outDataSize, const char *inData, uint64_t inDataSize, const header_t *head)
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

    header_t header = {
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

uint64_t responseLogData(const char *rawData, uint64_t rawDataSize, char *outData, uint64_t outDataSize, const struct Log_Data *pkg, uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj)
{
    if (outData == NULL || outDataSize == 0 || pkg == NULL)
        return 0;

    // Пустые данные
    if (rawData == NULL || rawDataSize == 0)
    {
        struct Log_Data logData;
        memcpy(&logData, pkg, sizeof(logData));
        logData.dataSize = 0;
        logData.data = NULL;

        header_t header = {
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
        return  writeFunc(outData, resultSize, obj);
    }

    uint64_t pkgsCount;
    uint64_t totalSendByte = 0;

    char *it = (char *)rawData;
    char *end = (char *)rawData + rawDataSize;

    pkgsCount = pkgCount(rawDataSize, outDataSize, HEADER_SIZE + LOG_DATA_SIZE + pkg->nameSize);
    for (uint16_t i = 0; i < pkgsCount; ++i)
    {
        uint64_t rawDataPart = outDataSize - HEADER_SIZE - LOG_DATA_SIZE - pkg->nameSize;
        uint64_t sizeLeft = end - it;
        if (sizeLeft < rawDataPart)
            rawDataPart = sizeLeft;

        struct Log_Data logData;
        memcpy(&logData, pkg, sizeof(logData));
        logData.dataSize = rawDataPart;
        logData.data = it;

        header_t header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_LOG_DATA,
            .flags = HEADER_FLAGS_NONE,
            .fragment = (uint16_t)(i + 1),
            .fragments = (uint16_t)(pkgsCount),
            .dataSize = logDataSize(&logData),
            .pkg = &logData
        };

        it += rawDataPart;

        uint64_t resultSize = headerToData(&header, outData, outDataSize);
        totalSendByte += writeFunc(outData, resultSize, obj);
    }

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

    header_t header = {
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

static uint64_t responceReadWritePkgCount(uint64_t dataOutSize, const iotv_obj_t *iot, const header_t *header)
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

    header_t header = {
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
