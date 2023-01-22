#include "identification.h"

uint64_t identificationCheckSum(const struct Identification *body)
{
    if (body == NULL)
        return 0;

    return  body->id + body->nameSize + body->descriptionSize
            + body->numberWriteChannel + body->numberReadChannel
            + body->flags;
}

uint64_t identificationSize(const struct Identification *body)
{
    if (body == NULL)
        return 0;

    return IDENTIFICATION_SIZE ;
}
