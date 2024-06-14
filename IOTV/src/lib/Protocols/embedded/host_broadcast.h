#ifndef HOST_BROADCAST_H
#define HOST_BROADCAST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct Host_Broadcast
{
    uint32_t address;
    uint16_t port;
    uint8_t nameSize;
    uint8_t flags;
    char *name;
};
typedef struct Host_Broadcast host_broadcast_t;

uint64_t hostBroadCastCheckSum(const host_broadcast_t *body);
uint64_t hostBroadCastSize(const host_broadcast_t *body);
uint64_t hostBroadCastToData(const host_broadcast_t *body, char *outData, uint64_t outDataSize);
void clearHostBroadCast(host_broadcast_t *host_broadcast);

host_broadcast_t *hostBroadCastCopy(host_broadcast_t *hostBroadCast);

#ifdef __cplusplus
}
#endif

#endif // HOST_BROADCAST_H
