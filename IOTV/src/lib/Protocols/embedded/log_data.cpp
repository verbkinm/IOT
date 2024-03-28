#include "log_data.h"
#include "iotv_types.h"
#include "string.h"
#include "stdlib.h"

uint64_t logDataCheckSum(const struct Log_Data *body)
{
    if (body == NULL)
        return 0;

    return  body->nameSize + body->startInterval + body->endInterval + body->interval + body->flags + body->channelNumber + body->dataSize;
}

uint64_t logDataSize(const struct Log_Data *body)
{
    if (body == NULL)
        return 0;

    return LOG_DATA_SIZE + body->nameSize + body->dataSize;
}

uint64_t logDataToData(const struct Log_Data *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) )
        return 0;

    if (outDataSize < logDataSize(body))
        return 0;

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
