#include "identification.h"
#include "iotv_types.h"

#include <string.h>
#include <stdlib.h>

uint64_t identificationCheckSum(const struct Identification *body)
{
    if (body == NULL)
        return 0;

    return  body->id + body->nameSize + body->descriptionSize + body->numberWriteChannel + body->numberReadChannel + body->flags;
}

uint64_t identificationSize(const struct Identification *body)
{
    if (body == NULL)
        return 0;

    return IDENTIFICATION_SIZE + body->nameSize + body->descriptionSize + body->numberWriteChannel + body->numberReadChannel;
}

uint64_t identificationToData(const struct Identification *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) )
        return 0;

    if (outDataSize < identificationSize(body))
        return 0;

    outData[0] = body->id >> 8;
    outData[1] = body->id;
    outData[2] = body->nameSize;
    outData[3] = body->descriptionSize >> 8;
    outData[4] = body->descriptionSize;
    outData[5] = body->numberWriteChannel;
    outData[6] = body->numberReadChannel;
    outData[7] = body->flags;

    uint64_t chSum =  body->id + body->nameSize + body->descriptionSize + body->numberWriteChannel + body->numberReadChannel + body->flags;

    memcpy(&outData[8], &chSum, 8);

    memcpy(&outData[IDENTIFICATION_SIZE], body->name, body->nameSize);
    memcpy(&outData[IDENTIFICATION_SIZE + body->nameSize], body->description, body->descriptionSize);

    memcpy(&outData[IDENTIFICATION_SIZE + body->nameSize + body->descriptionSize], body->writeChannelType, body->numberWriteChannel);
    memcpy(&outData[IDENTIFICATION_SIZE + body->nameSize + body->descriptionSize + body->numberWriteChannel], body->readChannelType, body->numberReadChannel);

    return IDENTIFICATION_SIZE + body->nameSize + body->descriptionSize + body->numberWriteChannel + body->numberReadChannel;
}

void clearIdentification(struct Identification *ident)
{
    if (ident == NULL)
        return;

    if (ident->name != NULL)
    {
        free((void *)ident->name);
        ident->name = NULL;
    }
    if (ident->description != NULL)
    {
        free((void *)ident->description);
        ident->description = NULL;
    }
    if (ident->writeChannelType != NULL)
    {
        free((void *)ident->writeChannelType);
        ident->writeChannelType = NULL;
    }
    if (ident->readChannelType != NULL)
    {
        free((void *)ident->readChannelType);
        ident->readChannelType = NULL;
    }

    free(ident);
//    ident = NULL;
}
