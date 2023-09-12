#include "iotvp_print.h"

void printHeader(const struct Header *head)
{
    assert(head != NULL);

    printf("----------------------------------------\n");

    printf("<HEADER>\n");
    printf("version - 0x%02x\n", head->version);
    printf("type - 0x%02x\n", head->type);
    printf("assignment - 0x%02x\n", head->assignment);
    printf("flags - 0x%02x\n", head->flags);

    printf("dataSize - 0x%016lx\n", head->dataSize);

//    if (head->identification != NULL)
//        printIdentification(head->identification);
//    if (head->state != NULL)
//        printState(head->state);
//    if (head->readWrite != NULL)
//        printReadWrite(head->readWrite);

    printf("----------------------------------------\n");
}

void printIdentification(const struct Identification *ident)
{
    assert(ident != NULL);

    printf("\t<Identification>\n");
    printf("\tid - 0x%04x\n", ident->id);
    printf("\tnameSize - 0x%02x\n", ident->nameSize);
    printf("\tdescriptionSize - 0x%04x\n", ident->descriptionSize);
    printf("\tnumberWriteChannel - 0x%02x\n", ident->numberWriteChannel);
    printf("\tnumberReadChannel - 0x%02x\n", ident->numberReadChannel);

    printf("\tname - ");
    for (int i = 0; i < ident->nameSize; ++i)
        printf("%c", ident->name[i]);
    printf("\n");

    printf("\tdescription - ");
    for (int i = 0; i < ident->descriptionSize; ++i)
        printf("%c", ident->description[i]);
    printf("\n");
}

void printState(const struct State *state)
{
    assert(state != NULL);

    printf("\t<State>\n");
    printf("\tnameSize - 0x%02x\n", state->nameSize);
    printf("\tstate - 0x%04x\n", state->state);
    printf("\tflags - 0x%02x\n", state->flags);
    printf("\tdataSize - 0x%08lx\n", state->dataSize);

    printf("\tname - ");
    for (int i = 0; i < state->nameSize; ++i)
        printf("%c", state->name[i]);
    printf("\n");
}

void printReadWrite(const struct Read_Write *readWrite)
{
    assert(readWrite != NULL);
    printf("\t<Read_Write>\n");
    printf("\tnameSize - 0x%02x\n", readWrite->nameSize);
    printf("\tchannelNumber - 0x%04x\n", readWrite->channelNumber);
    printf("\tflags - 0x%02x\n", readWrite->flags);
    printf("\tdataSize - 0x%08lx\n", readWrite->dataSize);

    printf("\tname - ");
    for (int i = 0; i < readWrite->nameSize; ++i)
        printf("%c", readWrite->name[i]);
    printf("\n");
}
