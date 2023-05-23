#ifndef TECH_H
#define TECH_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "iotv_types.h"

struct Tech
{
    const Tech_FLAGS flags;
    const Tech_TYPE type;

    const uint64_t dataSize;

    const uint8_t *data;
};

uint64_t techCheckSum(const struct Tech *body);
uint64_t techSize(const struct Tech *body);
uint64_t techToData(const struct Tech *body, char *outData, uint64_t outDataSize);
void clearTech(struct Tech *tech);

#endif // TECH_H
