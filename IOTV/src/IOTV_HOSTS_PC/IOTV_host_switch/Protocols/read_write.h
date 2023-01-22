#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <stdint.h>
#include <stdlib.h>

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

    //!!!
    char* const name;
    uint8_t* const data;
};

uint64_t readWriteCheckSum(const struct Read_Write *);
uint64_t readWriteSize(const struct Read_Write *);


#endif // READ_WRITE_H
