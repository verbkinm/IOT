#include "IOTV_SH.h"
#include "identification.h"
#include "read_write.h"
#include "state.h"
#include "tech.h"
#include "iotv_types.h"

#include <string.h>

static uint64_t responceReadWritePkgCount(uint64_t dataOutSize, const struct IOTV_Server_embedded *iot, const struct Header *header);

uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot)
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
        .flags = Identification_FLAGS_NONE,
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
            .nameSize = iot->nameSize,
            .channelNumber = rwPkg->channelNumber,
            .flags = rwPkg->flags,
            .dataSize = dataReadSize,
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

//uint64_t responseReadData(char *outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, const struct Header *head)
//{
//    if (head == NULL || outData == NULL || iot == NULL || head->pkg == NULL || iot->readChannel == NULL)
//        return 0;

//    struct Read_Write *ptrReadWrite = ((struct Read_Write *)head->pkg);

//    if (iot->numberReadChannel <= ptrReadWrite->channelNumber)
//        return 0;

//    if (ptrReadWrite->flags == ReadWrite_FLAGS_IGNORE_CH)
//    {
//        uint64_t resultDataSize = 0;
//        for(uint8_t ch_num = 0; ch_num < iot->numberReadChannel; ++ch_num)
//        {
//            uint64_t contrSize = dataSizeonDataType(iot->readChannelType[ch_num]);
//            if (contrSize == 0)
//                continue;

//            struct Read_Write readWrite = {
//                .nameSize = iot->nameSize,
//                .channelNumber = ch_num,
//                .flags = ReadWrite_FLAGS_NONE,
//                .dataSize = contrSize,
//                .name = iot->name,
//                .data = iot->readChannel[ch_num].data
//            };

//            struct Header header = {
//                .version = 2,
//                .type = HEADER_TYPE_RESPONSE,
//                .assignment = HEADER_ASSIGNMENT_READ,
//                .flags = HEADER_FLAGS_NONE,
//                .fragment = head->fragment,
//                .fragments = head->fragments,
//                .dataSize = readWriteSize(&readWrite),
//                .pkg = &readWrite,
//            };

//            resultDataSize += headerToData(&header, outData + resultDataSize, dataSize - resultDataSize);
//        }
//        return resultDataSize;
//    }

//    struct Read_Write readWrite = {
//        .nameSize = iot->nameSize,
//        .channelNumber = ptrReadWrite->channelNumber,
//        .flags = ReadWrite_FLAGS_NONE,
//        .dataSize = (uint64_t)dataSizeonDataType(iot->readChannelType[ptrReadWrite->channelNumber]),
//        .name = iot->name,
//        .data = iot->readChannel[ptrReadWrite->channelNumber].data
//    };

//    struct Header header = {
//        .version = 2,
//        .type = HEADER_TYPE_RESPONSE,
//        .assignment = HEADER_ASSIGNMENT_READ,
//        .flags = HEADER_FLAGS_NONE,
//        .fragment = head->fragment,
//        .fragments = head->fragments,
//        .dataSize = readWriteSize(&readWrite),
//        .pkg = &readWrite,
//    };

//    return headerToData(&header, outData, dataSize);
//}

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
        .nameSize = iot->nameSize,
        .channelNumber = ptrReadWrite->channelNumber,
        .flags = ReadWrite_FLAGS_NONE,
        .dataSize = iot->readChannel[ptrReadWrite->channelNumber].dataSize,
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
        .nameSize = (uint8_t)strlen(name),
        .channelNumber = channelNumber,
        .flags =ReadWrite_FLAGS_NONE,
        .dataSize = dataWriteSize,
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
        .nameSize = (uint8_t)strlen(name),
        .channelNumber = channelNumber,
        .flags = flags,
        .dataSize = 0,
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
