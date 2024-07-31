#include "log_data.h"
#include "iotv_types.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

uint64_t logDataCheckSum(const log_data_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return  body->nameSize + body->startInterval + body->endInterval + body->interval + body->flags + body->channelNumber + body->dataSize;
}

uint64_t logDataSize(const log_data_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return LOG_DATA_SIZE + body->nameSize + body->dataSize;
}

uint64_t logDataToData(const log_data_t *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) || (outDataSize < logDataSize(body)))
    {
        RETURN_WARNING;
        return 0;
    }

    outData[0] = body->nameSize;

    uint64_t startInterval =  body->startInterval;
    memcpy((void *)&outData[1], &startInterval, 8); // 8 - документация

    uint64_t endInterval =  body->endInterval;
    memcpy((void *)&outData[9], &endInterval, 8); // 8 - документация

    uint32_t interval =  body->interval;
    memcpy((void *)&outData[17], &interval, 4); // 4 - документация

    outData[21] = body->flags;
    outData[22] = body->channelNumber;

    uint32_t dataSize = body->dataSize;
    memcpy((void *)&outData[23], &dataSize, 4); // 4 - документация

    uint64_t chSum = body->nameSize + body->startInterval + body->endInterval + body->interval + body->flags + body->channelNumber + body->dataSize;;
    memcpy(&outData[27], &chSum, 8);

    if (body->nameSize > 0 && body->name != NULL)
        memcpy(&outData[LOG_DATA_SIZE], body->name, body->nameSize);
    if (body->dataSize > 0 && body->data != NULL)
        memcpy(&outData[LOG_DATA_SIZE + body->nameSize], body->data, body->dataSize);

    return LOG_DATA_SIZE + body->nameSize + body->dataSize;
}

void clearLogData(struct Log_Data *body)
{
    if (body == NULL)
        return;

    if (body->name != NULL)
        free((void *)body->name);
    if (body->data != NULL)
        free((void *)body->data);

    free(body);
}

log_data_t *logDataCopy(const log_data_t *logData)
{
    log_data_t *copy = NULL;

    if (logData == NULL)
        return copy;

    copy = calloc(1, sizeof(log_data_t));
    if (copy == NULL)
        return copy;

    memcpy(copy, logData, sizeof(log_data_t));

    copy->name = NULL;
    copy->data = NULL;

    if (logData->name != NULL && logData->nameSize > 0)
    {
        copy->name = malloc(logData->nameSize);
        if (copy->name != NULL)
            memcpy(copy->name, logData->name, logData->nameSize);
        else
            copy->nameSize = 0;
    }

    if (logData->data != NULL && logData->dataSize > 0)
    {
        copy->data = malloc(logData->dataSize);
        if (copy->data != NULL)
            memcpy(copy->data, logData->data, logData->dataSize);
        else
            copy->dataSize = 0;
    }

    return copy;
}
