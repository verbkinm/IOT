#include "state.h"
#include "iotv_types.h"

#include <stdlib.h>
#include <string.h>

uint64_t stateCheckSum(const struct State *body)
{
    if (body == NULL)
        return 0;

    return  body->nameSize + body->state + body->flags + body->dataSize;
}

uint64_t stateSize(const struct State *body)
{
    if (body == NULL)
        return 0;

    return STATE_SIZE + body->nameSize + body->dataSize;
}

uint64_t stateToData(const struct State *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) )
        return 0;

    if (outDataSize < stateSize(body))
        return 0;

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

void clearState(struct State *state)
{
    if (state == NULL)
        return;

    if (state->name != NULL)
        free((void *)state->name);
    if (state->data != NULL)
        free((void *)state->data);

    free(state);
//    state = NULL;
}
