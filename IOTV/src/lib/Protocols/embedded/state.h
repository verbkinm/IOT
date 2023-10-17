#ifndef STATE_H
#define STATE_H

#include <stdint.h>

struct State
{
    uint8_t nameSize;
    uint8_t state;
    uint8_t flags;

    uint64_t dataSize;

    char *name;
    uint8_t *data;
};

uint64_t stateCheckSum(const struct State *body);
uint64_t stateSize(const struct State *body);
uint64_t stateToData(const struct State *body, char *outData, uint64_t outDataSize);
void clearState(struct State *state);

#endif // STATE_H
