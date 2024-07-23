#include "read_write.h"

#include "iotv_types.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

uint64_t readWriteCheckSum(const read_write_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return  body->nameSize + body->channelNumber + body->flags + body->dataSize;
}

uint64_t readWriteSize(const read_write_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return READ_WRITE_SIZE + body->nameSize + body->dataSize;
}

uint64_t readWriteToData(const read_write_t *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) || (outDataSize < readWriteSize(body)))
    {
        RETURN_WARNING;
        return 0;
    }

    outData[0] = body->nameSize;
    outData[1] = body->channelNumber;
    outData[2] = body->flags;

    uint32_t dataSize =  body->dataSize;
    memcpy((void *)&outData[3], &dataSize, 4); // 4 - документация

    uint64_t chSum =  body->nameSize + body->channelNumber + body->flags + body->dataSize;
    memcpy(&outData[7], &chSum, 8);

    if ( (body->nameSize > 0)
        && (body->name != NULL)
        && (body->nameSize <= (outDataSize - READ_WRITE_SIZE)) )
    {
        memcpy(&outData[READ_WRITE_SIZE], body->name, body->nameSize);
    }

    if ((body->dataSize > 0)
        && (body->data != NULL)
        && (body->dataSize <= (outDataSize - READ_WRITE_SIZE - body->nameSize)))
    {
        memcpy(&outData[READ_WRITE_SIZE + body->nameSize], body->data, body->dataSize);
    }

    return READ_WRITE_SIZE + body->nameSize + body->dataSize;
}

void clearReadWrite(read_write_t *readWrite)
{
    if (readWrite == NULL)
        return;

    if (readWrite->name != NULL)
        free((void *)readWrite->name);
    if (readWrite->data != NULL)
        free((void *)readWrite->data);

    free(readWrite);
}

read_write_t *readWriteCopy(const read_write_t *readWrite_pkg)
{
    read_write_t *copy = NULL;

    if (readWrite_pkg == NULL)
        return copy;

    copy = calloc(1, sizeof(read_write_t));
    if (copy == NULL)
        return copy;

    memcpy(copy, readWrite_pkg, sizeof(read_write_t));

    copy->name = NULL;
    copy->data = NULL;

    if (readWrite_pkg->name != NULL && readWrite_pkg->nameSize > 0)
    {
        copy->name = malloc(copy->nameSize);
        if (copy->name != NULL)
            memcpy(copy->name, readWrite_pkg->name, copy->nameSize);
    }

    if (readWrite_pkg->data != NULL && readWrite_pkg->dataSize > 0)
    {
        copy->data = malloc(copy->dataSize);
        if (copy->data != NULL)
            memcpy(copy->data, readWrite_pkg->data, copy->dataSize);
    }

    return copy;
}
