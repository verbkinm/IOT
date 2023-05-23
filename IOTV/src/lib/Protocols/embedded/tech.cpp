#include "tech.h"
#include <stdint.h>

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
    if ( (body == NULL) || (outData == NULL) )
        return 0;

    if (outDataSize < techSize(body))
        return 0;

    outData[0] = body->type;
//    outData[1] = body->state;
    outData[2] = body->flags;

    uint32_t dataSize =  body->dataSize;
    memcpy(&outData[3], &dataSize, 4); // 4 - документация

    uint64_t chSum =  (uint8_t)body->type + body->flags + body->dataSize;
    memcpy(&outData[7], &chSum, 8); // 8 - документация

    memcpy(&outData[STATE_SIZE], body->data, body->dataSize);

    return TECH_SIZE + body->dataSize;
}

void clearTech(struct Tech *tech)
{
    if (tech == NULL)
        return;

    if (tech->data != NULL)
        free((void *)tech->data);

    free(tech);
    tech = NULL;
}

