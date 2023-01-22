#include "state.h"

uint64_t stateCheckSum(const struct States *body)
{
    if (body == NULL)
        return 0;

    return  body->nameSize + body->state
            + body->flags + body->dataSize;
}

uint64_t stateSize(const struct States *body)
{
    if (body == NULL)
        return 0;

    return STATE_SIZE + body->nameSize + body->dataSize;
}
