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
    } flags;

    enum State_STATE
    {
        State_STATE_OFFLINE = 0,
        State_STATE_ONLINE
    } state;

    uint8_t nameSize;
    uint64_t dataSize;

    char *name;
    uint8_t *data;
};

uint64_t stateCheckSum(const struct State *);
uint64_t stateSize(const struct State *);
uint64_t stateToData(struct State *body, char *outData, uint64_t outDataSize);

#endif // STATE_H
