#ifndef IOTV_SERVER_EMBEDDED_H
#define IOTV_SERVER_EMBEDDED_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

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

struct RawEmbedded {
    uint32_t dataSize;
    char *data;
};
typedef struct RawEmbedded raw_embedded_t;

struct IOTV_Server_embedded
{
    uint16_t id;

    uint8_t numberReadChannel;
    uint8_t numberWriteChannel;

    uint8_t state;

    uint8_t nameSize;
    uint16_t descriptionSize;

    raw_embedded_t *readChannel;
    uint8_t *readChannelType;
    uint8_t *writeChannelType;

    char *name;
    char *description;
};
typedef struct IOTV_Server_embedded iotv_obj_t;

int8_t dataSizeonDataType(uint8_t type);
bool byteOrderReversebleData(uint8_t type);
void clear_iotv_obj(iotv_obj_t *iot);

iotv_obj_t *iotCopy(const iotv_obj_t *iot);

uint64_t dataPart(char **data, uint64_t partNumber, uint64_t partSize, const iotv_obj_t *iot, uint8_t channelNumber);

#ifdef __cplusplus
}
#endif

#endif // IOTV_SERVER_EMBEDDED_H
