#ifndef IDENTIFICATION_H
#define IDENTIFICATION_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "iotv_types.h"

struct Identification
{
    Identification_FLAGS flags;

    const uint16_t id;
    const uint8_t nameSize;
    const uint16_t descriptionSize;

    const uint8_t numberWriteChannel;
    const uint8_t numberReadChannel;

    const char *name;
    const char *description;

    const uint8_t *writeChannelType;
    const uint8_t *readChannelType;
};

uint64_t identificationCheckSum(const struct Identification *);
uint64_t identificationSize(const struct Identification *);
uint64_t identificationToData(const struct Identification *body, char *outData, uint64_t outDataSize);
void clearIdentification(struct Identification *ident);

#endif // IDENTIFICATION_H

