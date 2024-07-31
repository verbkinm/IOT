#include "state.h"
#include "iotv_types.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

uint64_t stateCheckSum(const state_pkg_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return  body->nameSize + body->state + body->flags + body->dataSize;
}

uint64_t stateSize(const state_pkg_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return STATE_SIZE + body->nameSize + body->dataSize;
}

uint64_t stateToData(const state_pkg_t *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) || (outDataSize < stateSize(body)))
    {
        RETURN_WARNING;
        return 0;
    }

    outData[0] = body->nameSize;
    outData[1] = body->state;
    outData[2] = body->flags;

    uint32_t dataSize =  body->dataSize;
    memcpy(&outData[3], &dataSize, 4); // 4 - документация

    uint64_t chSum =  body->nameSize + body->state + body->flags + body->dataSize;
    memcpy(&outData[7], &chSum, 8);

    memcpy(&outData[STATE_SIZE], body->name, body->nameSize);
    memcpy(&outData[STATE_SIZE + body->nameSize], body->data, body->dataSize);

    return STATE_SIZE + body->nameSize + body->dataSize;
}

void clearState(state_pkg_t *state)
{
    if (state == NULL)
        return;

    if (state->name != NULL)
        free((void *)state->name);
    if (state->data != NULL)
        free((void *)state->data);

    free(state);
}

state_pkg_t *stateCopy(const state_pkg_t *state_pkg)
{
    state_pkg_t *copy = NULL;

    if (state_pkg == NULL)
        return copy;

    copy = calloc(1, sizeof(state_pkg_t));
    if (copy == NULL)
        return copy;

    memcpy(copy, state_pkg, sizeof(state_pkg_t));

    copy->name = NULL;
    copy->data = NULL;

    if (state_pkg->name != NULL && state_pkg->nameSize > 0)
    {
        copy->name = malloc(copy->nameSize);
        if (copy->name != NULL)
            memcpy(copy->name, state_pkg->name, copy->nameSize);
    }

    if (state_pkg->data != NULL && state_pkg->dataSize > 0)
    {
        copy->data = malloc(copy->dataSize);
        if (copy->data != NULL)
            memcpy(copy->data, state_pkg->data, copy->dataSize);
    }

    return copy;
}
