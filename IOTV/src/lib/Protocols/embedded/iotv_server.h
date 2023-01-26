#ifndef IOTV_SERVER_H
#define IOTV_SERVER_H

#define READ_CHANNEL_LENGTH 3
#define WRITE_CHANNEL_LENGTH 3

#include <stdint.h>
#include <stdlib.h>

enum DATA_TYPE
{
    DATA_TYPE_INT_8 = 0,
    DATA_TYPE_INT_16,
    DATA_TYPE_INT_32,
    DATA_TYPE_INT_64,

    DATA_TYPE_FLOAT_32,
    DATA_TYPE_DOUBLE_64, // на МК double 32-битный может быть

    DATA_TYPE_BOOL,
    DATA_TYPE_STRING,

    DATA_TYPE_RAW,
    DATA_TYPE_NONE
};

struct Raw {
    uint32_t dataSize;
    char *data;
};

struct IOTV_Server
{
    const uint8_t id;
    const char *name;
    const char *description;

    const uint8_t numberReadChannel;
    struct Raw *readChannel;
    const uint8_t *readChannelType;

    const uint8_t numberWriteChannel;
    const uint8_t *writeChannelType;

    uint8_t state;
};

int8_t dataSizeonDataType(enum DATA_TYPE type);
void clearIOTV_Server(struct IOTV_Server *iot);

#endif // IOTV_SERVER_H
