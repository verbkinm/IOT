#include "header.h"

uint64_t headerCheckSum(const struct Header *header)
{
    if (header == NULL)
        return 0;

    return  header->version + header->type + header->assignment
            + header->flags + header->dataSize;
}

uint64_t headerDataSize(const struct Header *header)
{
    if (header == NULL)
        return 0;

    if (header->identification != NULL)
        return identificationSize(header->identification);
    if (header->readWrite != NULL)
        return readWriteSize(header->readWrite);
    if (header->state != NULL)
        return stateSize(header->state);

    return 0;
}

uint64_t headerSize(struct Header *header)
{
    if (header == NULL)
        return 0;

    return HEADER_SIZE + headerDataSize(header);
}

struct Header* createPkgs(uint8_t* const data, uint64_t size, bool *ok,
                          uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (ok == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    if (size < HEADER_SIZE)
    {
        *expectedDataSize = HEADER_SIZE;
        return NULL;
    }

    uint64_t bodySize = 0;
    memcpy(&data[4], &bodySize, 8); // Размер тела пакета (документация)
    if (isLittleEndian())
        dataReverse(&bodySize, sizeof(bodySize));

    if (size < (HEADER_SIZE + bodySize))
    {
        *expectedDataSize = HEADER_SIZE + bodySize;
        return NULL;
    }

    struct Header *header = malloc(sizeof(struct Header));

    return header;
}

bool isLittleEndian()
{
    uint16_t value = 0x1;
    if ( *((char *) &value) == 1)
        return true;

    return false;
}
void dataReverse(void* data, uint64_t size)
{
    if (data == NULL)
        return;

    for (uint64_t i = 0; i < size/2; i++)
    {
        uint8_t temp = ((uint8_t*)(data))[i];
        ((uint8_t*)(data))[i] = ((uint8_t*)(data))[size - 1 - i];
        ((uint8_t*)(data))[size - 1 - i] = temp;
    }
}

