#include "IOTV_SH.h"

uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot)
{
    if (outData == NULL || iot == NULL)
        return 0;

    struct Identification ident = {
        .flags = Identification::Identification_FLAGS_NONE,
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
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_IDENTIFICATION,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = identificationSize(&ident),
        .identification = &ident,
        .readWrite = NULL,
        .state = NULL
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t responsePingData(char* outData, uint64_t dataSize)
{
    assert(outData != NULL);

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

    return headerToData(&header, outData, dataSize);
}

uint64_t responseReadData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, const struct Header *head)
{
    assert(outData != NULL);
    assert(iot != NULL);
    assert(head != NULL);
    assert(head->readWrite != NULL);

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = iot->nameSize,
        .channelNumber = head->readWrite->channelNumber,
        .dataSize = iot->readChannel[head->readWrite->channelNumber].dataSize,
        .name = iot->name,
        .data = iot->readChannel[head->readWrite->channelNumber].data
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

    return headerToData(&header, outData, dataSize);
}

uint64_t responseWriteData(char* outData, uint64_t dataSize, struct IOTV_Server_embedded *iot, const Header *head)
{
    assert(outData != NULL);
    assert(iot != NULL);
    assert(head != NULL);
    assert(head->readWrite != NULL);

    //!!! Для каждого устройства своё настраивать?
    assert(iot->readChannel != NULL);
    assert(iot->readChannel[head->readWrite->channelNumber].data != NULL);

    if (head->readWrite->dataSize > 0)
        memcpy(iot->readChannel[head->readWrite->channelNumber].data, head->readWrite->data,
                iot->readChannel[head->readWrite->channelNumber].dataSize);

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = iot->nameSize,
        .channelNumber = head->readWrite->channelNumber,
        .dataSize = iot->readChannel[head->readWrite->channelNumber].dataSize,
        .name = iot->name,
        .data = iot->readChannel[head->readWrite->channelNumber].data
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

    return headerToData(&header, outData, dataSize);
}

uint64_t responseStateData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot)
{
    if (outData == NULL || iot == NULL)
        return 0;

    struct State state = {
        .flags = State::STATE_FLAGS_NONE,
        .state = static_cast<State::State_STATE>(iot->state),
        .nameSize = iot->nameSize,
        .dataSize = 0,
        .name = iot->name,
        .data = NULL
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_STATE,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = stateSize(&state),
        .identification = NULL,
        .readWrite = NULL,
        .state = &state
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t queryIdentificationData(char* outData, uint64_t dataSize)
{
    if (outData == NULL)
        return 0;

    struct Header header = {
        .type = Header::HEADER_TYPE_REQUEST,
        .assignment = Header::HEADER_ASSIGNMENT_IDENTIFICATION,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = 0,
        .identification = NULL,
        .readWrite = NULL,
        .state = NULL
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t queryPingData(char* outData, uint64_t dataSize)
{
    if (outData == NULL)
        return 0;

    struct Header header = {
        .type = Header::HEADER_TYPE_REQUEST,
        .assignment = Header::HEADER_ASSIGNMENT_PING_PONG,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = 0,
        .identification = NULL,
        .readWrite = NULL,
        .state = NULL
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t queryWriteData(char* outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, const char *dataToWrite, uint32_t dataWriteSize)
{
    if (outData == NULL || dataToWrite == NULL)
        return 0;

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = static_cast<uint8_t>(strlen(name)),
        .channelNumber = channelNumber,
        .dataSize = dataWriteSize,
        .name = name,
        .data = dataToWrite
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_REQUEST,
        .assignment = Header::HEADER_ASSIGNMENT_WRITE,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = readWriteSize(&readWrite),
        .identification = NULL,
        .readWrite = &readWrite,
        .state = NULL
    };

    return headerToData(&header, outData, outDataSize);
}

uint64_t queryReadData(char* outData, uint64_t outDataSize, const char *name, uint8_t channelNumber)
{
    if (outData == NULL)
        return 0;

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = static_cast<uint8_t>(strlen(name)),
        .channelNumber = channelNumber,
        .dataSize = 0,
        .name = name,
        .data = NULL
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_REQUEST,
        .assignment = Header::HEADER_ASSIGNMENT_READ,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = readWriteSize(&readWrite),
        .identification = NULL,
        .readWrite = &readWrite,
        .state = NULL
    };

    return headerToData(&header, outData, outDataSize);
}

uint64_t queryStateData(char* outData, uint64_t outDataSize, const char *name)
{
    if (outData == NULL)
        return 0;

    struct State state = {
        .flags = State::STATE_FLAGS_NONE,
        .state = State::State_STATE_OFFLINE,
        .nameSize = static_cast<uint8_t>(strlen(name)),
        .dataSize = 0,
        .name = name,
        .data = NULL
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_REQUEST,
        .assignment = Header::HEADER_ASSIGNMENT_STATE,
        .flags = Header::HEADER_FLAGS_NONE,
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
    assert(header != NULL);
    assert(header->identification != NULL);
//    assert(header->identification->readChannelType != NULL);
//    assert(header->identification->writeChannelType != NULL);

    auto nameSize = header->identification->nameSize;
    auto descriptionSize = header->identification->descriptionSize;

    auto numberWriteChannel = header->identification->numberWriteChannel;
    auto numberReadChannel = header->identification->numberReadChannel;

    struct IOTV_Server_embedded iot = {
        .id = header->identification->id,
        .name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL,
        .description = (descriptionSize > 0) ? (char *)malloc(descriptionSize) : NULL,
        .numberReadChannel = numberReadChannel,
        .readChannel = (numberReadChannel > 0) ? (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * numberReadChannel) : NULL, //! Нет проверки, если не ошибка выделения
        .readChannelType = (numberReadChannel > 0) ? (uint8_t *)malloc(numberReadChannel) : NULL, //! Нет проверки, если не ошибка выделения
        .numberWriteChannel = header->identification->numberWriteChannel,
        .writeChannelType = (numberWriteChannel > 0) ? (uint8_t *)malloc(numberWriteChannel) : NULL, //! Нет проверки, если не ошибка выделения
        .state = false,
        .nameSize = nameSize,
        .descriptionSize = descriptionSize
    };

    if (nameSize > 0)
        memcpy((void *)iot.name, header->identification->name, nameSize);
    if (descriptionSize > 0)
        memcpy((void *)iot.description, header->identification->description, descriptionSize);

    for (int i = 0; i < numberReadChannel; ++i)
    {
        iot.readChannel[i].dataSize = dataSizeonDataType(header->identification->readChannelType[i]);
        iot.readChannel[i].data = (char *)malloc(iot.readChannel[i].dataSize);
        iot.readChannelType[i] = header->identification->readChannelType[i];
        iot.writeChannelType[i] = header->identification->writeChannelType[i];
    }

    struct IOTV_Server_embedded *ptrIot = (struct IOTV_Server_embedded *)malloc(sizeof(struct IOTV_Server_embedded));
    memcpy(ptrIot, &iot, sizeof(struct IOTV_Server_embedded));

    return ptrIot;
}

