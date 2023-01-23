#include "header.h"

uint64_t headerCheckSum(const struct Header *header)
{
    if (header == NULL)
        return 0;

    return  header->version + header->type + header->assignment
            + header->flags + header->dataSize;
}

uint64_t headerDataSize(const struct Header *header)
{
    if (header == NULL)
        return 0;

    if (header->identification != NULL)
        return HEADER_SIZE + identificationSize(header->identification);
    if (header->readWrite != NULL)
        return HEADER_SIZE + readWriteSize(header->readWrite);
    if (header->state != NULL)
        return HEADER_SIZE + stateSize(header->state);

    return 0;
}

uint64_t headerSize(struct Header *header)
{
    if (header == NULL)
        return 0;

    return HEADER_SIZE + headerDataSize(header);
}

uint64_t headerToData(struct Header *header, char *outData, uint64_t outDataSize)
{
    if ( (header == NULL) || (outData == NULL) )
        return 0;

    if (outDataSize < headerSize(header))
        return 0;

    uint64_t result = HEADER_SIZE;

    outData[0] = header->version;
    outData[1] = header->type;
    outData[2] = header->assignment;
    outData[3] = header->flags;

    uint64_t dataSize = header->dataSize;
    if (isLittleEndian())
        dataReverse(&dataSize, sizeof(dataSize));
    memcpy(&outData[4], &dataSize, 8);

    uint64_t chSum = header->version + header->type + header->assignment + header->flags + header->dataSize;
    if (isLittleEndian())
        dataReverse(&chSum, sizeof(chSum));
    memcpy(&outData[12], &chSum, 8);

    if (header->identification != NULL)
        result += identificationToData(header->identification, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
    else if (header->readWrite != NULL)
        result += readWriteToData(header->readWrite, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
    else if (header->state != NULL)
        result += stateToData(header->state, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);

    return result;
}
