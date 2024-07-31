#include "iotv_types.h"

#include "header.h"
#include "tech.h"
#include "identification.h"
#include "read_write.h"
#include "host_broadcast.h"
#include "state.h"
#include "log_data.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t headerCheckSum(const header_t *header)
{
    if (header == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return  header->version + header->type + header->assignment + header->flags + header->fragment + header->fragments + header->dataSize;
}

uint64_t headerDataSize(const header_t *header)
{
    if (header == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    if (header->pkg != NULL)
    {
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
    }

    return 0;
}

uint64_t headerSize(const header_t *header)
{
    if (header == NULL)
    {
        RETURN_WARNING;
        return 0;
    }
    return HEADER_SIZE + headerDataSize(header);
}

uint64_t headerToData(const header_t *header, char *outData, uint64_t outDataSize)
{
    if (header == NULL || outData == NULL || (outDataSize < headerSize(header)))
    {
        RETURN_WARNING;
        return 0;
    }

    uint64_t result = HEADER_SIZE;

    outData[0] = header->version;
    outData[1] = header->type;
    outData[2] = header->assignment;
    outData[3] = header->flags;

    memcpy(&outData[4], &header->fragment, 2);
    memcpy(&outData[6], &header->fragments, 2);
    memcpy(&outData[8], &header->dataSize, 8);

    uint64_t chSum = header->version + header->type + header->assignment + header->flags + header->fragment + header->fragments + header->dataSize;
    memcpy(&outData[16], &chSum, 8);

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
        case HEADER_ASSIGNMENT_HOST_BROADCAST:
            result += hostBroadCastToData((const struct Host_Broadcast *)header->pkg, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
            break;
        case HEADER_ASSIGNMENT_LOG_DATA:
            result += logDataToData((const struct Log_Data *)header->pkg, &outData[HEADER_SIZE], outDataSize - HEADER_SIZE);
            break;
        default:
            break;
        }
    }

    return result;
}

void clearHeader(header_t *header)
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
        case HEADER_ASSIGNMENT_LOG_DATA:
            clearLogData((struct Log_Data *)header->pkg);
            break;
        default:
            break;
        }
    }

    free(header);
}

uint64_t pkgCount(uint64_t sendDataSize, uint64_t buffSize, uint64_t offsetSize)
{
    uint64_t maxDatainFrame = buffSize - offsetSize;

    if (maxDatainFrame == 0 || buffSize < offsetSize)
    {
        RETURN_WARNING;
        return 0;
    }

    uint64_t result = sendDataSize / maxDatainFrame;

    if (sendDataSize % maxDatainFrame)
        ++result;

    return result;
}

header_t *headerCopy(header_t *header)
{
    header_t *copy = NULL;

    if (header == NULL)
        return copy;

    copy = calloc(1, sizeof(header_t));
    if (copy == NULL)
        return copy;

    memcpy(copy, header, sizeof(header_t));

    copy->pkg = NULL;


    if (copy->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
        copy->pkg = identificationCopy(header->pkg);
    else if (header->assignment == HEADER_ASSIGNMENT_STATE)
        copy->pkg = stateCopy(header->pkg);
    else if (header->assignment == HEADER_ASSIGNMENT_READ || header->assignment == HEADER_ASSIGNMENT_WRITE)
        copy->pkg = readWriteCopy(header->pkg);
    else if (header->assignment == HEADER_ASSIGNMENT_TECH)
        copy->pkg = techCopy(header->pkg);
    else if (header->assignment == HEADER_ASSIGNMENT_HOST_BROADCAST)
        copy->pkg = hostBroadCastCopy(header->pkg);
    else if (header->assignment == HEADER_ASSIGNMENT_LOG_DATA)
        copy->pkg = logDataCopy(header->pkg);
    else
        copy->pkg = NULL;


    return copy;
}
