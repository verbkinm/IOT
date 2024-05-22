#include "read_write.h"

#include "iotv_types.h"
#include "string.h"
#include "stdlib.h"

uint64_t readWriteCheckSum(const struct Read_Write *body)
{
    if (body == NULL)
        return 0;

    return  body->nameSize + body->channelNumber + body->flags + body->dataSize;
}

uint64_t readWriteSize(const struct Read_Write *body)
{
    if (body == NULL)
        return 0;

    return READ_WRITE_SIZE + body->nameSize + body->dataSize;
}

uint64_t readWriteToData(const struct Read_Write *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) )
        return 0;

    if (outDataSize < readWriteSize(body))
        return 0;

    outData[0] = body->nameSize;
    outData[1] = body->channelNumber;
    outData[2] = body->flags;

    uint32_t dataSize =  body->dataSize;
    memcpy((void *)&outData[3], &dataSize, 4); // 4 - документация

    uint64_t chSum =  body->nameSize + body->channelNumber + body->flags + body->dataSize;
    memcpy(&outData[7], &chSum, 8);

    if ( (body->nameSize > 0)
        && (body->name != NULL)
        && (body->nameSize < (outDataSize - READ_WRITE_SIZE)) )
    {
        memcpy(&outData[READ_WRITE_SIZE], body->name, body->nameSize);
    }

    if ((body->dataSize > 0)
        && (body->data != NULL)
        && (body->dataSize < (outDataSize - READ_WRITE_SIZE - body->nameSize)))
    {
        memcpy(&outData[READ_WRITE_SIZE + body->nameSize], body->data, body->dataSize);
    }

    return READ_WRITE_SIZE + body->nameSize + body->dataSize;
}

void clearReadWrite(struct Read_Write *readWrite)
{
    if (readWrite == NULL)
        return;

    if (readWrite->name != NULL)
        free((void *)readWrite->name);
    if (readWrite->data != NULL)
        free((void *)readWrite->data);

    free(readWrite);
}
