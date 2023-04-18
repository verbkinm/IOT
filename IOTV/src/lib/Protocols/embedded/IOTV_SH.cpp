#include "IOTV_SH.h"
#include "identification.h"
#include "read_write.h"
#include "state.h"

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
                    .identification = NULL,
                    .readWrite = NULL,
                    .state = NULL
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
                .identification = &ident,
                .readWrite = NULL,
                .state = NULL
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
                .identification = NULL,
                .readWrite = NULL,
                .state = NULL
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t responseReadData(char *outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, const struct Header *head)
{
    if (outData == NULL || iot == NULL || head == NULL || head->readWrite == NULL
            || (iot->readChannel != NULL && (iot->numberReadChannel <= head->readWrite->channelNumber)))
        return 0;

    struct Read_Write readWrite = {
        .flags = ReadWrite_FLAGS_NONE,
                .nameSize = iot->nameSize,
                .channelNumber = head->readWrite->channelNumber,
                .dataSize = (iot->readChannel != NULL) ? iot->readChannel[head->readWrite->channelNumber].dataSize : 0,
                .name = iot->name,
                .data = (iot->readChannel != NULL) ? iot->readChannel[head->readWrite->channelNumber].data : NULL
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

    return headerToData(&header, outData, dataSize);
}

uint64_t responseWriteData(char *outData, uint64_t dataSize, struct IOTV_Server_embedded *iot, const struct Header *head)
{
    if (outData == NULL || iot == NULL || iot->readChannel == NULL || head == NULL || head->readWrite == NULL
            || iot->numberWriteChannel <= head->readWrite->channelNumber
            || iot->numberReadChannel <= head->readWrite->channelNumber
            || iot->readChannel[head->readWrite->channelNumber].data == NULL)
        return 0;

    if (head->readWrite->dataSize > 0)
        memcpy(iot->readChannel[head->readWrite->channelNumber].data, head->readWrite->data, iot->readChannel[head->readWrite->channelNumber].dataSize);

    struct Read_Write readWrite = {
        .flags = ReadWrite_FLAGS_NONE,
                .nameSize = iot->nameSize,
                .channelNumber = head->readWrite->channelNumber,
                .dataSize = iot->readChannel[head->readWrite->channelNumber].dataSize,
                .name = iot->name,
                .data = iot->readChannel[head->readWrite->channelNumber].data
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
                .identification = NULL,
                .readWrite = NULL,
                .state = &state
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
                .identification = NULL,
                .readWrite = NULL,
                .state = NULL
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
                .identification = NULL,
                .readWrite = NULL,
                .state = NULL
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
                .identification = NULL,
                .readWrite = &readWrite,
                .state = NULL
    };

    return headerToData(&header, outData, outDataSize);
}

uint64_t queryReadData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber)
{
    if (outData == NULL)
        return 0;

    struct Read_Write readWrite = {
        .flags = ReadWrite_FLAGS_NONE,
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
                .identification = NULL,
                .readWrite = &readWrite,
                .state = NULL
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
                .identification = NULL,
                .readWrite = NULL,
                .state = &state
    };

    return headerToData(&header, outData, outDataSize);
}

struct IOTV_Server_embedded *createIotFromHeaderIdentification(const struct Header *header)
{
    if (header == NULL || header->identification == NULL)
        return NULL;

    uint8_t nameSize = header->identification->nameSize;
    uint16_t descriptionSize = header->identification->descriptionSize;

    uint8_t numberWriteChannel = header->identification->numberWriteChannel;
    uint8_t numberReadChannel = header->identification->numberReadChannel;

    struct IOTV_Server_embedded *ptrIot = (struct IOTV_Server_embedded *)malloc(sizeof(struct IOTV_Server_embedded));
    if (ptrIot == NULL)
        return NULL;

    struct IOTV_Server_embedded iot = {
        .id = header->identification->id,
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
        memcpy((void *)iot.name, header->identification->name, nameSize);
    if (descriptionSize > 0)
        memcpy((void *)iot.description, header->identification->description, descriptionSize);

    bool mallocFlag = false;
    for (int i = 0; i < numberReadChannel; ++i)
    {
        iot.readChannel[i].dataSize = dataSizeonDataType(header->identification->readChannelType[i]);
        iot.readChannel[i].data = (char *)malloc(iot.readChannel[i].dataSize);
        if (iot.readChannel[i].dataSize > 0 && iot.readChannel[i].data == NULL)
        {
            // не удалось выделить память!
            mallocFlag = true;
            break;
        }
        iot.readChannelType[i] = header->identification->readChannelType[i];
    }

    for (int i = 0; i < numberWriteChannel; ++i)
        iot.writeChannelType[i] = header->identification->writeChannelType[i];

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

