#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <stdint.h>
#include <stdlib.h>

static const uint64_t STATE_SIZE = 15;

struct States
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

    //!!!
    char* const name;
    uint8_t* const data;
};

uint64_t stateCheckSum(const struct States *);
uint64_t stateSize(const struct States *);


#endif // READ_WRITE_H
