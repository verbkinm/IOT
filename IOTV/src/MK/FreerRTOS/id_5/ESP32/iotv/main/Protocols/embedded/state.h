#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "iotv_types.h"

struct State
{
    const State_FLAGS flags;
    const State_STATE state;

    const uint8_t nameSize;
    const uint64_t dataSize;

    const char *name;
    const uint8_t *data;
};

uint64_t stateCheckSum(const struct State *body);
uint64_t stateSize(const struct State *body);
uint64_t stateToData(const struct State *body, char *outData, uint64_t outDataSize);
void clearState(struct State *state);

#endif // STATE_H
