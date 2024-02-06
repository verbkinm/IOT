#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <stdint.h>

struct Read_Write
{
    uint8_t nameSize;
    uint8_t channelNumber;
    uint8_t flags;

    uint64_t dataSize;

    char *name;
    char *data;
};

uint64_t readWriteCheckSum(const struct Read_Write *body);
uint64_t readWriteSize(const struct Read_Write *body);
uint64_t readWriteToData(const struct Read_Write *body, char *outData, uint64_t outDataSize);
void clearReadWrite(struct Read_Write *readWrite);

#endif // READ_WRITE_H