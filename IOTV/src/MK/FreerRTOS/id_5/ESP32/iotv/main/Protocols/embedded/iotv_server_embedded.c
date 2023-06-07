#include "iotv_server_embedded.h"

int8_t dataSizeonDataType(uint8_t type)
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
        return 4;
    case DATA_TYPE_INT_64:
    case DATA_TYPE_DOUBLE_64:
        return 8;
    case DATA_TYPE_STRING:
    case DATA_TYPE_RAW:
    case DATA_TYPE_NONE:
    default:
        return 0;
        break;
    }
}

bool byteOrderReversebleData(uint8_t type)
{
    switch (type)
    {
    case DATA_TYPE_INT_16:
    case DATA_TYPE_INT_32:
    case DATA_TYPE_INT_64:
        return true;
    case DATA_TYPE_BOOL:
    case DATA_TYPE_INT_8:
    case DATA_TYPE_FLOAT_32:
    case DATA_TYPE_DOUBLE_64:
    case DATA_TYPE_STRING:
    case DATA_TYPE_RAW:
    case DATA_TYPE_NONE:
        return false;
    default:
        return false;
        break;
    }
}

void clearIOTV_Server(struct IOTV_Server_embedded *iot)
{
    if (iot == NULL)
        return;

    if (iot->name != NULL)
        free((void *)iot->name);
    if (iot->description != NULL)
        free((void *)iot->description);

    if (iot->readChannel != NULL)
    {
        for (uint8_t i = 0; i < iot->numberReadChannel; ++i)
        {
            if (iot->readChannel[i].data != NULL)
                free(iot->readChannel[i].data);
        }
        free(iot->readChannel);
    }
    if (iot->readChannelType != NULL)
        free(iot->readChannelType);
    if (iot->writeChannelType != NULL)
        free(iot->writeChannelType);

    free(iot);
}

