#include "IOTV_SH.h"

uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot)
{
    if (outData == NULL || iot == NULL)
        return 0;

    struct Identification ident = {
        .flags = Identification::Identification_FLAGS_NONE,
        .id = iot->id,
        .nameSize = static_cast<uint8_t>(strlen(iot->name)),
        .descriptionSize = static_cast<uint16_t>(strlen(iot->description)),
        .numberWriteChannel = WRITE_CHANNEL_LENGTH,
        .numberReadChannel = READ_CHANNEL_LENGTH,
        .name = iot->name,
        .description = iot->description,
        .writeChannelType = (const uint8_t *)&iot->writeChannelType,
        .readChannelType = (const uint8_t *)&iot->readChannelType
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_IDENTIFICATION,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = HEADER_SIZE + identificationSize(&ident),
        .identification = &ident,
        .readWrite = NULL,
        .state = NULL
    };

    return headerToData(&header, outData, dataSize);
}

uint64_t responsePingData(char* outData, uint64_t dataSize)
{
    if (outData == NULL)
        return 0;

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

uint64_t responseReadData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot, const struct Header *head)
{
    if (outData == NULL || iot == NULL || head == NULL)
        return 0;

    if (head->readWrite == NULL)
        return 0;

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = static_cast<uint8_t>(strlen(iot->name)),
        .channelNumber = head->readWrite->channelNumber,
        .dataSize = sizeof(iot->readChannel[0]),
        .name = iot->name,
        .data = (const uint8_t *)&iot->readChannel[head->readWrite->channelNumber]
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

    return headerToData(&header, outData, dataSize);
}

uint64_t responseWriteData(char* outData, uint64_t dataSize, struct IOTV_Server *iot, const Header *head)
{
    if (outData == NULL || iot == NULL || head == NULL)
        return 0;

    if (head->readWrite == NULL)
        return 0;

    struct Read_Write readWrite = {
        .flags = Read_Write::ReadWrite_FLAGS_NONE,
        .nameSize = static_cast<uint8_t>(strlen(iot->name)),
        .channelNumber = head->readWrite->channelNumber,
        .dataSize = 0,
        .name = iot->name,
        .data = NULL
    };

    struct Header header = {
        .type = Header::HEADER_TYPE_RESPONSE,
        .assignment = Header::HEADER_ASSIGNMENT_WRITE,
        .flags = Header::HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = HEADER_SIZE + readWriteSize(&readWrite),
        .identification = NULL,
        .readWrite = &readWrite,
        .state = NULL
    };

    //!!! Для каждого устройства своё настраивать?
    iot->readChannel[header.readWrite->channelNumber] = *head->readWrite->data;

    return headerToData(&header, outData, dataSize);
}
