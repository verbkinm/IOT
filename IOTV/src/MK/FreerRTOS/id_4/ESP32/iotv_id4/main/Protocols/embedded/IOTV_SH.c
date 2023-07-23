#include "IOTV_SH.h"
#include "identification.h"
#include "read_write.h"
#include "state.h"
#include "tech.h"

//!!! проверку на NULL



uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot)
{
    if (outData == NULL)
        return 0;

    if (iot == NULL)
    {
        struct Header header = {
            .type = HEADER_TYPE_RESPONSE,
                    .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
                    .flags = HEADER_FLAGS_NONE,
                    .version = 2,
                    .dataSize = 0,
                    .pkg = NULL
        };

        return headerToData(&header, outData, dataSize);
    }

    struct Identification ident = {
        .flags = Identification_FLAGS_NONE,
                .id = iot->id,
                .nameSize = iot->nameSize,
                .descriptionSize = iot->descriptionSize,
                .numberWriteChannel = iot->numberWriteChannel,
                .numberReadChannel = iot->numberReadChannel,
                .name = iot->name,
                .description = iot->description,
                .writeChannelType = iot->writeChannelType,
                .readChannelType = iot->readChannelType
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
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
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_PING_PONG,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
                .dataSize = 0,
                .pkg = NULL
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t responseReadData(char *outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, const struct Header *head)
{
    if (head == NULL || outData == NULL || iot == NULL || head->pkg == NULL)
        return 0;

    struct Read_Write *ptrReadWrite = ((struct Read_Write *)head->pkg);

    if (iot->readChannel != NULL && (iot->numberReadChannel <= ptrReadWrite->channelNumber))
        return 0;

    if (ptrReadWrite->flags == ReadWrite_FLAGS_IGNORE_CH)
    {
        uint64_t resultDataSize = 0;
        for(uint8_t ch_num = 0; ch_num < iot->numberReadChannel; ++ch_num)
        {
            struct Read_Write readWrite = {
                .flags = ReadWrite_FLAGS_NONE,
                        .nameSize = iot->nameSize,
                        .channelNumber = ch_num,
                        .dataSize = (iot->readChannel != NULL) ? iot->readChannel[ch_num].dataSize : 0,
                        .name = iot->name,
                        .data = (iot->readChannel != NULL) ? iot->readChannel[ch_num].data : NULL
            };

            struct Header header = {
                .type = HEADER_TYPE_RESPONSE,
                        .assignment = HEADER_ASSIGNMENT_READ,
                        .flags = HEADER_FLAGS_NONE,
                        .version = 2,
                        .dataSize = readWriteSize(&readWrite),
                        .pkg = &readWrite,
            };

            resultDataSize += headerToData(&header, outData + resultDataSize, dataSize - resultDataSize);
        }
        return resultDataSize;
    }

    struct Read_Write readWrite = {
        .flags = ReadWrite_FLAGS_NONE,
                .nameSize = iot->nameSize,
                .channelNumber = ptrReadWrite->channelNumber,
                .dataSize = (iot->readChannel != NULL) ? iot->readChannel[ptrReadWrite->channelNumber].dataSize : 0,
                .name = iot->name,
                .data = (iot->readChannel != NULL) ? iot->readChannel[ptrReadWrite->channelNumber].data : NULL
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_READ,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
                .dataSize = readWriteSize(&readWrite),
                .pkg = &readWrite,
    };

    return headerToData(&header, outData, dataSize);
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
        if (iot->readChannel[ptrReadWrite->channelNumber].dataSize != ptrReadWrite->dataSize)
            return 0;

        memcpy(iot->readChannel[ptrReadWrite->channelNumber].data, ptrReadWrite->data, iot->readChannel[ptrReadWrite->channelNumber].dataSize);
    }

    struct Read_Write readWrite = {
        .flags = ReadWrite_FLAGS_NONE,
                .nameSize = iot->nameSize,
                .channelNumber = ptrReadWrite->channelNumber,
                .dataSize = iot->readChannel[ptrReadWrite->channelNumber].dataSize,
                .name = iot->name,
                .data = iot->readChannel[ptrReadWrite->channelNumber].data
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_WRITE,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
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
        .flags = STATE_FLAGS_NONE,
                .state = (State_STATE)iot->state,
                .nameSize = iot->nameSize,
                .dataSize = 0,
                .name = iot->name,
                .data = NULL
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_STATE,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
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
        .type = HEADER_TYPE_REQUEST,
                .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
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
        .type = HEADER_TYPE_REQUEST,
                .assignment = HEADER_ASSIGNMENT_PING_PONG,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
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
        .flags =ReadWrite_FLAGS_NONE,
                .nameSize = (uint8_t)strlen(name),
                .channelNumber = channelNumber,
                .dataSize = dataWriteSize,
                .name = name,
                .data = dataToWrite
    };

    struct Header header = {
        .type = HEADER_TYPE_REQUEST,
                .assignment = HEADER_ASSIGNMENT_WRITE,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
                .dataSize = readWriteSize(&readWrite),
                .pkg = &readWrite,
    };

    return headerToData(&header, outData, outDataSize);
}

uint64_t queryReadData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, ReadWrite_FLAGS flags)
{
    if (outData == NULL)
        return 0;

    struct Read_Write readWrite = {
        .flags = flags,
                .nameSize = (uint8_t)strlen(name),
                .channelNumber = channelNumber,
                .dataSize = 0,
                .name = name,
                .data = NULL
    };

    struct Header header = {
        .type = HEADER_TYPE_REQUEST,
                .assignment = HEADER_ASSIGNMENT_READ,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
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
        .flags = STATE_FLAGS_NONE,
                .state = State_STATE_OFFLINE,
                .nameSize = (uint8_t)strlen(name),
                .dataSize = 0,
                .name = name,
                .data = NULL
    };

    struct Header header = {
        .type = HEADER_TYPE_REQUEST,
                .assignment = HEADER_ASSIGNMENT_STATE,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
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
                .name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL,
                .description = (descriptionSize > 0) ? (char *)malloc(descriptionSize) : NULL,
                .numberReadChannel = numberReadChannel,
                .readChannel = (numberReadChannel > 0) ? (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * numberReadChannel) : NULL,
                .readChannelType = (numberReadChannel > 0) ? (uint8_t *)malloc(numberReadChannel) : NULL,
                .numberWriteChannel = numberWriteChannel,
                .writeChannelType = (numberWriteChannel > 0) ? (uint8_t *)malloc(numberWriteChannel) : NULL,
                .state = false,
                .nameSize = nameSize,
                .descriptionSize = descriptionSize
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


uint64_t responseTech(char *outData, uint64_t outDataSize, const char *inData, uint64_t inDataSize, const struct Header *head)
{
    if (outData == NULL || inData == NULL || head == NULL || head->pkg == NULL)
        return 0;

    const struct Tech *pkg = (const struct Tech *)(head->pkg);

    struct Tech tech = {
        .flags = Tech_FLAGS_NONE,
        .type = pkg->type,
        .dataSize = inDataSize,
        .data = (const uint8_t*)inData
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
                .assignment = HEADER_ASSIGNMENT_TECH,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
                .dataSize = techSize(&tech),
                .pkg = &tech
    };

    return headerToData(&header, outData, outDataSize);
}

uint64_t queryTech(char *outData, uint64_t dataSize, const char *inData, uint64_t inDataSize, Tech_TYPE type)
{
    if (outData == NULL || (inData == NULL && inDataSize > 0))
        return 0;

    struct Tech tech = {
        .flags = Tech_FLAGS_NONE,
        .type = type,
        .dataSize = inDataSize,
        .data = (const uint8_t *)inData
    };

    struct Header header = {
        .type = HEADER_TYPE_REQUEST,
                .assignment = HEADER_ASSIGNMENT_TECH,
                .flags = HEADER_FLAGS_NONE,
                .version = 2,
                .dataSize = techSize(&tech),
                .pkg = &tech
    };

    return headerToData(&header, outData, dataSize);
}
