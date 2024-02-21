#ifndef HOST_BROADCAST_H
#define HOST_BROADCAST_H

#include <stdint.h>

struct Host_Broadcast
{
    uint32_t address;
    uint16_t port;
    uint8_t nameSize;
    uint8_t flags;
    char *name;
};

uint64_t hostBroadCastCheckSum(const struct Host_Broadcast *body);
uint64_t hostBroadCastSize(const struct Host_Broadcast *body);
uint64_t hostBroadCastToData(const struct Host_Broadcast *body, char *outData, uint64_t outDataSize);
void clearHostBroadCast(struct Host_Broadcast *host_broadcast);

#endif // HOST_BROADCAST_H
