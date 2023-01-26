#ifndef IOTV_SERVER_H
#define IOTV_SERVER_H

#define READ_CHANNEL_LENGTH 3
#define WRITE_CHANNEL_LENGTH 3

#include <stdint.h>

struct IOTV_Server
{
    const uint8_t id;
    const char *name;
    const char *description;

    uint32_t readChannel[READ_CHANNEL_LENGTH];
    uint8_t readChannelType[READ_CHANNEL_LENGTH];

    uint8_t writeChannelType[WRITE_CHANNEL_LENGTH];

    uint8_t state;
};

#endif // IOTV_SERVER_H
