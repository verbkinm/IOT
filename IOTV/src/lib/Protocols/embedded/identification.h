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

    uint16_t id;
    uint8_t nameSize;
    uint16_t descriptionSize;

    uint8_t numberWriteChannel;
    uint8_t numberReadChannel;

    char *name;
    char *description;

    uint8_t *writeChannelType;
    uint8_t *readChannelType;
};

uint64_t identificationCheckSum(const struct Identification *);
uint64_t identificationSize(const struct Identification *);
uint64_t identificationToData(struct Identification *body, char *outData, uint64_t outDataSize);

#endif // IDENTIFICATION_H

