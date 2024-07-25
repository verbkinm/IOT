#include "identification.h"
#include "iotv_types.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint64_t identificationCheckSum(const identification_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return  body->id + body->nameSize + body->descriptionSize + body->numberWriteChannel + body->numberReadChannel + body->flags;
}

uint64_t identificationSize(const identification_t *body)
{
    if (body == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    return IDENTIFICATION_SIZE + body->nameSize + body->descriptionSize + body->numberWriteChannel + body->numberReadChannel;
}

uint64_t identificationToData(const identification_t *body, char *outData, uint64_t outDataSize)
{
    if ( (body == NULL) || (outData == NULL) || (outDataSize < identificationSize(body)))
    {
        RETURN_WARNING;
        return 0;
    }

    outData[0] = body->id >> 8;
    outData[1] = body->id;
    outData[2] = body->nameSize;
    outData[3] = body->numberWriteChannel;
    outData[4] = body->numberReadChannel;
    outData[5] = body->descriptionSize >> 8;
    outData[6] = body->descriptionSize;
    outData[7] = body->flags;

    uint64_t chSum =  body->id + body->nameSize + body->descriptionSize + body->numberWriteChannel + body->numberReadChannel + body->flags;

    memcpy(&outData[8], &chSum, 8);

    memcpy(&outData[IDENTIFICATION_SIZE], body->name, body->nameSize);
    memcpy(&outData[IDENTIFICATION_SIZE + body->nameSize], body->description, body->descriptionSize);

    memcpy(&outData[IDENTIFICATION_SIZE + body->nameSize + body->descriptionSize], body->writeChannelType, body->numberWriteChannel);
    memcpy(&outData[IDENTIFICATION_SIZE + body->nameSize + body->descriptionSize + body->numberWriteChannel], body->readChannelType, body->numberReadChannel);

    return IDENTIFICATION_SIZE + body->nameSize + body->descriptionSize + body->numberWriteChannel + body->numberReadChannel;
}

void clearIdentification(identification_t *ident)
{
    if (ident == NULL)
        return;

    if (ident->name != NULL)
        free((void *)ident->name);
    if (ident->description != NULL)
        free((void *)ident->description);
    if (ident->writeChannelType != NULL)
        free((void *)ident->writeChannelType);
    if (ident->readChannelType != NULL)
        free((void *)ident->readChannelType);

    free(ident);
}

identification_t *identificationCopy(const identification_t *ident)
{
    identification_t *copy = NULL;

    if (ident == NULL)
        return copy;

    copy = calloc(1, sizeof(identification_t));
    if (copy == NULL)
    {
        RETURN_WARNING;
        return copy;
    }

    memcpy(copy, ident, sizeof(identification_t));

    copy->name = NULL;
    copy->description = NULL;
    copy->writeChannelType = NULL;
    copy->readChannelType = NULL;

    if (ident->name != NULL && ident->nameSize > 0)
    {
        copy->name = malloc(ident->nameSize);
        if (copy->name != NULL)
            memcpy(copy->name, ident->name, ident->nameSize);
        else
            copy->nameSize = 0;
    }

    if (ident->description != NULL && ident->descriptionSize > 0)
    {
        copy->description = malloc(ident->descriptionSize);
        if (copy->description != NULL)
            memcpy(copy->description, ident->description, ident->descriptionSize);
        else
            copy->descriptionSize = 0;
    }

    if (ident->writeChannelType != NULL && ident->numberWriteChannel > 0)
    {
        copy->writeChannelType = malloc(ident->numberWriteChannel);
        if (copy->writeChannelType != NULL)
            memcpy(copy->writeChannelType, ident->writeChannelType, ident->numberWriteChannel);
        else
            copy->numberWriteChannel = 0;
    }

    if (ident->readChannelType != NULL && ident->numberReadChannel > 0)
    {
        copy->readChannelType = malloc(ident->numberReadChannel);
        if (copy->readChannelType != NULL)
            memcpy(copy->readChannelType, ident->readChannelType, ident->numberReadChannel);
        else
            copy->numberReadChannel = 0;
    }

    return copy;
}
