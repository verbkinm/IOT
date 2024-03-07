#include "tech.h"
#include "iotv_types.h"

#include <string.h>
#include <stdlib.h>

uint64_t techCheckSum(const struct Tech *body)
{
    if (body == NULL)
        return 0;

    return  (uint8_t)body->type + body->flags + body->dataSize;
}

uint64_t techSize(const struct Tech *body)
{
    if (body == NULL)
        return 0;

    return TECH_SIZE + body->dataSize;
}

uint64_t techToData(const struct Tech *body, char *outData, uint64_t outDataSize)
{
    if ( body == NULL || outData == NULL || (outDataSize < techSize(body)))
        return 0;

    outData[0] = body->type;
    outData[1] = 0; // Резер не задействован
    outData[2] = body->flags;

    uint32_t dataSize =  body->dataSize;
    memcpy(&outData[3], &dataSize, 4); // 4 - документация

    uint64_t chSum =  (uint8_t)body->type + body->flags + body->dataSize;
    memcpy(&outData[7], &chSum, 8); // 8 - документация

    memcpy(&outData[TECH_SIZE], body->data, body->dataSize);

    return TECH_SIZE + body->dataSize;
}

void clearTech(struct Tech *tech)
{
    if (tech == NULL)
        return;

    if (tech->data != NULL)
        free((void *)tech->data);

    free(tech);
}

