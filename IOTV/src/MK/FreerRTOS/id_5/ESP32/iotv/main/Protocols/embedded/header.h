#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct Header
{
    uint8_t version;
    uint8_t type;
    uint8_t assignment;
    uint8_t flags;

    uint16_t fragment;
    uint16_t fragments;

    uint64_t dataSize;

    void *pkg;
};
typedef struct Header header_t;

uint64_t headerSize(const header_t *header);
uint64_t headerDataSize(const header_t *header);
uint64_t headerCheckSum(const header_t *header);
uint64_t headerToData(const header_t *header, char *outData, uint64_t outDataSize);
void clearHeader(header_t *header);

uint64_t pkgCount(uint64_t sendDataSize, uint64_t buffSize, uint64_t offsetSize);

#ifdef __cplusplus
}
#endif

#endif // HEADER_H
