#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "creatorpkgs.h"

static const uint64_t HEADER_SIZE = 20;

struct Header
{
    enum Header_TYPE
    {
        HEADER_TYPE_NONE = 0,
        HEADER_TYPE_REQUEST,
        HEADER_TYPE_RESPONSE
    } const type;

    enum Header_ASSIGNMENT
    {
        HEADER_ASSIGNMENT_NONE = 0,
        HEADER_ASSIGNMENT_IDENTIFICATION,
        HEADER_ASSIGNMENT_STATE,
        HEADER_ASSIGNMENT_READ,
        HEADER_ASSIGNMENT_WRITE,
        HEADER_ASSIGNMENT_PING_PONG
    } const assignment;

    enum Header_FLAGS
    {
        HEADER_FLAGS_NONE = 0,
        HEADER_FLAGS_ERROR = 0xFF
    } const flags;

    const uint8_t version;
    const uint64_t dataSize;

    const struct Identification *identification;
    const struct Read_Write *readWrite;
    const struct State *state;
};

uint64_t headerSize(const struct Header *header);
uint64_t headerDataSize(const struct Header *header);
uint64_t headerCheckSum(const struct Header *header);
uint64_t headerToData(const struct Header *header, char *outData, uint64_t outDataSize);
void clearHeader(struct Header *header);

#endif // HEADER_H
