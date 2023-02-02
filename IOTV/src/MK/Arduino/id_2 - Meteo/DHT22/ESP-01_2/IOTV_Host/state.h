#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <stdlib.h>
#include "creatorpkgs.h"

static const uint64_t STATE_SIZE = 15;

struct State
{
    enum State_FLAGS
    {
        STATE_FLAGS_NONE = 0,
        STATE_FLAGS_ERROR = 0xFF
    } const flags;

    enum State_STATE
    {
        State_STATE_OFFLINE = 0,
        State_STATE_ONLINE
    } const state;

    const uint8_t nameSize;
    const uint64_t dataSize;

    const char *name;
    const uint8_t *data;
};

uint64_t stateCheckSum(const struct State *);
uint64_t stateSize(const struct State *);
uint64_t stateToData(const struct State *body, char *outData, uint64_t outDataSize);
void clearState(struct State *state);

#endif // STATE_H
