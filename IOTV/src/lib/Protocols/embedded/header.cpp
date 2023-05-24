#include "header.h"
#include "tech.h"
#include "identification.h"
#include "read_write.h"
#include "state.h"

uint64_t headerCheckSum(const struct Header *header)
{
    if (header == NULL)
        return 0;

    return  header->version + header->type + header->assignment + header->flags + header->dataSize;
}

uint64_t headerDataSize(const struct Header *header)
{
    if (header == NULL || header->pkg == NULL)
        return 0;

    // Должно быть что-то одно.
    switch (header->assignment)
    {
    case HEADER_ASSIGNMENT_IDENTIFICATION:
        return identificationSize((const struct Identification *)header->pkg);
        break;
    case HEADER_ASSIGNMENT_STATE:
        return stateSize((const struct State*)header->pkg);
        break;
    case HEADER_ASSIGNMENT_READ:
    case HEADER_ASSIGNMENT_WRITE:
        return readWriteSize((const struct Read_Write *)header->pkg);
        break;
    case HEADER_ASSIGNMENT_TECH:
        return techSize((const struct Tech *)header->pkg);
        break;
    default:
        break;
    }

    return 0;
}

uint64_t headerSize(const struct Header *header)
{
    if (header == NULL)
        return 0;

    return HEADER_SIZE + headerDataSize(header);
}

uint64_t headerToData(const struct Header *header, char *outData, uint64_t outDataSize)
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
    memcpy(&outData[4], &dataSize, 8);

    uint64_t chSum = header->version + header->type + header->assignment + header->flags + header->dataSize;
    memcpy(&outData[12], &chSum, 8);

    if (header->pkg != NULL)
    {
        switch (header->assignment)
        {
        case HEADER_ASSIGNMENT_IDENTIFICATION:
            result += identificationToData((const struct Identification *)header->pkg, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
            break;
        case HEADER_ASSIGNMENT_STATE:
            result += stateToData((const struct State *)header->pkg, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
            break;
        case HEADER_ASSIGNMENT_READ:
        case HEADER_ASSIGNMENT_WRITE:
            result += readWriteToData((const struct Read_Write *)header->pkg, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
            break;
        case HEADER_ASSIGNMENT_TECH:
            result += techToData((const struct Tech *)header->pkg, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
            break;
        default:
            break;
        }
    }
//    if (header->identification != NULL)
//        result += identificationToData(header->identification, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
//    else if (header->readWrite != NULL)
//        result += readWriteToData(header->readWrite, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
//    else if (header->state != NULL)
//        result += stateToData(header->state, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);

    return result;
}

void clearHeader(struct Header *header)
{
    if (header == NULL)
        return;

    if (header->pkg != NULL)
    {
        switch (header->assignment)
        {
        case HEADER_ASSIGNMENT_IDENTIFICATION:
            clearIdentification((struct Identification *)header->pkg);
            break;
        case HEADER_ASSIGNMENT_STATE:
            clearState((struct State *)header->pkg);
            break;
        case HEADER_ASSIGNMENT_READ:
        case HEADER_ASSIGNMENT_WRITE:
            clearReadWrite((struct Read_Write *)header->pkg);
            break;
        case HEADER_ASSIGNMENT_TECH:
            clearTech((struct Tech *)header->pkg);
            break;
        default:
            break;
        }
    }

//    if (header->identification != NULL)
//        clearIdentification((struct Identification *)header->identification);
//    if (header->readWrite != NULL)
//        clearReadWrite((struct Read_Write *)header->readWrite);
//    if (header->state != NULL)
//        clearState((struct State *)header->state);

    free(header);
//    header = NULL;
}
