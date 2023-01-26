#include "iotv_server.h"

int8_t dataSizeonDataType(enum DATA_TYPE type)
{
    switch (type)
    {
    case DATA_TYPE_BOOL:
    case DATA_TYPE_INT_8:
        return 1;
    case DATA_TYPE_INT_16:
        return 2;
    case DATA_TYPE_INT_32:
    case DATA_TYPE_FLOAT_32:
        return 3;
    case DATA_TYPE_INT_64:
    case DATA_TYPE_DOUBLE_64:
        return 4;
    case DATA_TYPE_STRING:
    case DATA_TYPE_RAW:
    case DATA_TYPE_NONE:
    default:
        return 0;
        break;
    }
}

void clearIOTV_Server(struct IOTV_Server *iot)
{
    if (iot == NULL)
        return;

    if (iot->readChannel != NULL)
        free(iot->readChannel);
    if (iot->name != NULL)
        free((void *)iot->name);
    if (iot->description != NULL)
        free((void *)iot->description);

    free(iot);
}
