#include "host_broadcast.h"

#include "iotv_types.h"
#include "string.h"
#include "stdlib.h"

uint64_t hostBroadCastCheckSum(const struct Host_Broadcast *body)
{
    if (body == NULL)
        return 0;

    return  body->nameSize + body->address + body->flags + body->port;
}

uint64_t hostBroadCastSize(const struct Host_Broadcast *body)
{
    if (body == NULL)
        return 0;

    return HOST_BROADCAST_SIZE + body->nameSize;
}

uint64_t hostBroadCastToData(const struct Host_Broadcast *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) )
        return 0;

    if (outDataSize < hostBroadCastSize(body))
        return 0;

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

void clearHostBroadCast(struct Host_Broadcast *host_broadcast)
{
    if (host_broadcast == NULL)
        return;

    if (host_broadcast->name != NULL)
        free((void *)host_broadcast->name);

    free(host_broadcast);
}
