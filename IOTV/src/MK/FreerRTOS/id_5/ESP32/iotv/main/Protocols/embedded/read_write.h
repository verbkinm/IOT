#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct Read_Write
{
    uint32_t dataSize;

    uint8_t nameSize;
    uint8_t channelNumber;
    uint8_t flags;

    char *name;
    char *data;
};
typedef struct Read_Write read_write_t;

uint64_t readWriteCheckSum(const read_write_t *body);
uint64_t readWriteSize(const read_write_t *body);
uint64_t readWriteToData(const read_write_t *body, char *outData, uint64_t outDataSize);
void clearReadWrite(read_write_t *readWrite);

read_write_t *readWriteCopy(const read_write_t *readWrite_pkg);

#ifdef __cplusplus
}
#endif

#endif // READ_WRITE_H
