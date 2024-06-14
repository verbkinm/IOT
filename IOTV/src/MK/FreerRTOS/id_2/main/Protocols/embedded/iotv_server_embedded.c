#include "iotv_server_embedded.h"

#include <stdio.h>

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
