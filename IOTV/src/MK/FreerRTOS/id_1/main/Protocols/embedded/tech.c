#include "tech.h"
#include "iotv_types.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

uint64_t techCheckSum(const tech_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return  (uint8_t)body->type + body->flags + body->dataSize;
}

uint64_t techSize(const tech_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return TECH_SIZE + body->dataSize;
}

uint64_t techToData(const tech_t *body, char *outData, uint64_t outDataSize)
{
    if ( body == NULL || outData == NULL || (outDataSize < techSize(body)))
    {
        RETURN_WARNING;
        return 0;
    }

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

void clearTech(tech_t *tech)
{
    if (tech == NULL)
        return;

    if (tech->data != NULL)
        free((void *)tech->data);

    free(tech);
}


tech_t *techCopy(tech_t *tech)
{
    tech_t *copy = NULL;

    if (tech == NULL)
        return copy;

    copy = calloc(1, sizeof(tech_t));
    if (copy == NULL)
        return copy;

    memcpy(copy, tech, sizeof(tech_t));

    copy->data = NULL;

    if (tech->data != NULL && tech->dataSize > 0)
    {
        copy->data = malloc(copy->dataSize);
        if (copy->data != NULL)
            memcpy(copy->data, tech->data, copy->dataSize);
    }

    return copy;
}
