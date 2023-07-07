#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "iotv_types.h"

struct Header
{
    const Header_TYPE type;
    const Header_ASSIGNMENT assignment;
    const Header_FLAGS flags;

    const uint8_t version;
    const uint64_t dataSize;

    const void *pkg;
};

uint64_t headerSize(const struct Header *header);
uint64_t headerDataSize(const struct Header *header);
uint64_t headerCheckSum(const struct Header *header);
uint64_t headerToData(const struct Header *header, char *outData, uint64_t outDataSize);
void clearHeader(struct Header *header);

#endif // HEADER_H
