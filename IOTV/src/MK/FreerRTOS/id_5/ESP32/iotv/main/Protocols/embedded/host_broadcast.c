#include "host_broadcast.h"

#include "iotv_types.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

uint64_t hostBroadCastCheckSum(const host_broadcast_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return  body->nameSize + body->address + body->flags + body->port;
}

uint64_t hostBroadCastSize(const host_broadcast_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return HOST_BROADCAST_SIZE + body->nameSize;
}

uint64_t hostBroadCastToData(const host_broadcast_t *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) || (outDataSize < hostBroadCastSize(body)))
    {
        RETURN_WARNING;
        return 0;
    }

    outData[0] = body->nameSize;

    uint32_t address =  body->address;
    memcpy((void *)&outData[1], &address, 4); // 4 - документация

    outData[5] = body->flags;

    uint16_t port =  body->port;
    memcpy((void *)&outData[6], &port, 2); // 2 - документация

    uint64_t chSum = body->nameSize + body->address + body->flags + body->port;
    memcpy(&outData[8], &chSum, 8);

    if (body->nameSize > 0 && body->name != NULL)
        memcpy(&outData[HOST_BROADCAST_SIZE], body->name, body->nameSize);

    return HOST_BROADCAST_SIZE + body->nameSize;
}

void clearHostBroadCast(host_broadcast_t *host_broadcast)
{
    if (host_broadcast == NULL)
        return;

    if (host_broadcast->name != NULL)
        free((void *)host_broadcast->name);

    free(host_broadcast);
}

host_broadcast_t *hostBroadCastCopy(host_broadcast_t *hostBroadCast)
{
    host_broadcast_t *copy = NULL;

    if (hostBroadCast == NULL)
        return copy;

    copy = calloc(1, sizeof(host_broadcast_t));
    if (copy == NULL)
        return copy;

    memcpy(copy, hostBroadCast, sizeof(host_broadcast_t));

    copy->name = NULL;

    if (hostBroadCast->name != NULL && hostBroadCast->nameSize > 0)
    {
        copy->name = malloc(copy->nameSize);
        if (copy->name != NULL)
            memcpy(copy->name, hostBroadCast->name, copy->nameSize);
    }

    return copy;
}
