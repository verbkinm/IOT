#ifndef IDENTIFICATION_H
#define IDENTIFICATION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct Identification
{
    uint16_t id;
    uint16_t descriptionSize;
    uint8_t nameSize;

    uint8_t numberWriteChannel;
    uint8_t numberReadChannel;
    uint8_t flags;

    char *name;
    char *description;

    uint8_t *writeChannelType;
    uint8_t *readChannelType;
};

uint64_t identificationCheckSum(const struct Identification *);
uint64_t identificationSize(const struct Identification *);
uint64_t identificationToData(const struct Identification *body, char *outData, uint64_t outDataSize);
void clearIdentification(struct Identification *ident);

#ifdef __cplusplus
}
#endif

#endif // IDENTIFICATION_H

