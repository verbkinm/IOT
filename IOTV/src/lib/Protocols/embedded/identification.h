#ifndef IDENTIFICATION_H
#define IDENTIFICATION_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "creatorpkgs.h"

static const uint64_t IDENTIFICATION_SIZE = 16;

struct Identification
{
    enum Identification_FLAGS
    {
        Identification_FLAGS_NONE = 0,
        Identification_FLAGS_ERROR = 0xFF
    } flags;

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
uint64_t identificationToData(struct Identification *body, char *outData, uint64_t outDataSize);

#endif // IDENTIFICATION_H

