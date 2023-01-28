#include "IOTV_SH.h"

uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot)
{
    if (outData == NULL || iot == NULL)
        return 0;

    struct Identification ident = {
        .flags = Identification::Identification_FLAGS_NONE,
        .id = iot->id,
        .nameSize = static_cast<uint8_t>(strlen(iot->name)),
        .descriptionSize = static_cast<uint16_t>(strlen(iot->description)),
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
        .nameSize = static_cast<uint8_t>(strlen(iot->name)),
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

    memmove(&iot->readChannel[head->readWrite->channelNumber].data, head->readWrite->data,
            iot->readChannel[head->readWrite->channelNumber].dataSize);

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = static_cast<uint8_t>(strlen(iot->name)),
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
        .nameSize = static_cast<uint8_t>(strlen(iot->name)),
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
