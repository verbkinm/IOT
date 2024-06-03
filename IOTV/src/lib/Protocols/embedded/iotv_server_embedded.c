#include "iotv_server_embedded.h"

#include <stdio.h>
#include <string.h>

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

void clear_iotv_obj(iotv_obj_t *iot)
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


uint64_t dataPart(char **data, uint64_t partNumber, uint64_t partSize, const iotv_obj_t *iot, uint8_t channelNumber)
{
    if (iot == NULL || iot->readChannel == NULL || channelNumber >= iot->numberReadChannel)
    {
        data = NULL;
        return 0;
    }

    char *dataChannel = iot->readChannel[channelNumber].data;
    uint64_t dataSize = iot->readChannel[channelNumber].dataSize;

    //    if (channelNumber == 15)
    //    {
    //    	printf("dataPart, dataSize = %d\n", (int)dataSize);
    //    	if (dataChannel != NULL)
    //    	{
    //			for (int i = 0; i < dataSize; ++i)
    //				printf("%c", dataChannel[i]);
    //			printf("\n");
    //    	}
    //    }

    char *endData = dataChannel + dataSize;

    *data = dataChannel + partSize * partNumber;

    // Выход за пределы
    if (*data >= endData)
    {
        *data = NULL;
        return 0;
    }

    // Осталось данных от текущего указателя data
    uint64_t sizeLeft = endData - *data;

    if (sizeLeft < partSize)
        return sizeLeft;

    return partSize;
}

iotv_obj_t *iotCopy(const iotv_obj_t *iot)
{
    iotv_obj_t *copy = NULL;

    if (iot == NULL)
        return copy;

    copy = calloc(1, sizeof(iotv_obj_t));
    if (copy == NULL)
        return copy;

    memcpy(copy, iot, sizeof(iotv_obj_t));

    copy->readChannel = NULL;
    copy->readChannelType = NULL;
    copy->writeChannelType = NULL;
    copy->name = NULL;
    copy->description = NULL;

    // readChannel
    if ((iot->readChannel != NULL) && (iot->numberReadChannel > 0))
    {
        copy->readChannel = calloc(1, sizeof(raw_embedded_t) * iot->numberReadChannel);
        if (copy->readChannel != NULL)
        {
            for (uint8_t i = 0; i < iot->numberReadChannel; i++)
            {
                if ((iot->readChannel[i].data != NULL) && (iot->readChannel[i].dataSize > 0))
                {
                    copy->readChannel[i].data = malloc(iot->readChannel[i].dataSize);
                    if (copy->readChannel[i].data != NULL)
                    {
                        copy->readChannel[i].dataSize = iot->readChannel[i].dataSize;
                        memcpy(copy->readChannel[i].data, iot->readChannel[i].data, copy->readChannel[i].dataSize);
                    }
                    else
                        copy->readChannel[i].dataSize = 0;
                }
            }
        }
    }

    // readChannelType
    if ((iot->readChannelType != NULL) && (iot->numberReadChannel > 0))
    {
        copy->readChannelType = malloc(iot->numberReadChannel);
        if (copy->readChannelType != NULL)
            memcpy(copy->readChannelType, iot->readChannelType, copy->numberReadChannel);
    }

    // writeChannelType
    if ((iot->writeChannelType != NULL) && (iot->numberWriteChannel > 0))
    {
        copy->writeChannelType = malloc(iot->numberWriteChannel);
        if (copy->writeChannelType != NULL)
            memcpy(copy->writeChannelType, iot->writeChannelType, copy->numberWriteChannel);
    }

    // name
    if ((iot->name != NULL) && (iot->nameSize > 0))
    {
        copy->name = malloc(iot->nameSize);
        if (copy->name != NULL)
            memcpy(copy->name, iot->name, copy->nameSize);
        else
            copy->nameSize = 0;
    }

    // description
    if ((iot->description != NULL) && (iot->descriptionSize > 0))
    {
        copy->description = malloc(iot->descriptionSize);
        if (copy->description != NULL)
            memcpy(copy->description, iot->description, copy->descriptionSize);
        else
            copy->descriptionSize = 0;
    }

    return copy;
}
