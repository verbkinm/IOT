#ifndef TECH_H
#define TECH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct Tech
{
    uint8_t flags;
    uint8_t type;

    uint64_t dataSize;

    uint8_t *data;
};
typedef struct Tech tech_t;

uint64_t techCheckSum(const tech_t *body);
uint64_t techSize(const tech_t *body);
uint64_t techToData(const tech_t *body, char *outData, uint64_t outDataSize);
void clearTech(tech_t *tech);

tech_t *techCopy(tech_t *tech);

#ifdef __cplusplus
}
#endif

#endif // TECH_H
