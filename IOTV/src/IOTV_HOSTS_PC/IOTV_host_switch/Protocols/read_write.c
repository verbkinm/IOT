#include "read_write.h"

uint64_t readWriteCheckSum(const struct Read_Write *body)
{
    if (body == NULL)
        return 0;

    return  body->nameSize + body->channelNumber
            + body->flags + body->dataSize;
}

uint64_t readWriteSize(const struct Read_Write *body)
{
    if (body == NULL)
        return 0;

    return READ_WRITE_SIZE + body->nameSize + body->dataSize;
}
