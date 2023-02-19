#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <stdint.h>
#include <stdlib.h>
#include "creatorpkgs.h"

static const uint64_t READ_WRITE_SIZE = 15;

struct Read_Write
{
    enum ReadWrite_FLAGS
    {
        ReadWrite_FLAGS_NONE = 0,
        ReadWrite_FLAGS_ERROR = 0xFF
    } const flags;

    const uint8_t nameSize;
    const uint8_t channelNumber;

    const uint64_t dataSize;

    const char *name;
    const char *data;
};

uint64_t readWriteCheckSum(const struct Read_Write *);
uint64_t readWriteSize(const struct Read_Write *);
uint64_t readWriteToData(const struct Read_Write *body, char *outData, uint64_t outDataSize);
void clearReadWrite(struct Read_Write *readWrite);

#endif // READ_WRITE_H
