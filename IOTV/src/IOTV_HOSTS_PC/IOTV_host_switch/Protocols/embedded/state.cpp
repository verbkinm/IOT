#include "state.h"

uint64_t stateCheckSum(const struct State *body)
{
    if (body == NULL)
        return 0;

    return  body->nameSize + body->state
            + body->flags + body->dataSize;
}

uint64_t stateSize(const struct State *body)
{
    if (body == NULL)
        return 0;

    return STATE_SIZE + body->nameSize + body->dataSize;
}

uint64_t stateToData(struct State *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) )
        return 0;

    if (outDataSize < stateSize(body))
        return 0;

    outData[0] = body->nameSize;
    outData[1] = body->state;
    outData[2] = body->flags;

    if (outDataSize < stateSize(body) + body->nameSize + body->dataSize)
        return 0;

    uint64_t dataSize =  body->dataSize;
    if (isLittleEndian())
        dataReverse(&dataSize, sizeof(dataSize));
    memcpy(&outData[3], &dataSize, 4);

    uint64_t chSum =  body->nameSize + body->state + body->flags + body->dataSize;
    if (isLittleEndian())
        dataReverse(&chSum, sizeof(chSum));
    memcpy(&outData[7], &chSum, 8);

    memcpy(&outData[STATE_SIZE], body->name, body->nameSize);
    memcpy(&outData[STATE_SIZE + body->nameSize], body->data, body->dataSize);

    return STATE_SIZE + body->nameSize + body->dataSize;
}
