#include "creatorpkgs.h"

struct Header* createPkgs(uint8_t* const data, uint64_t size, bool *error,
                          uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    struct Header *header = createHeader(data, size, error, expectedDataSize, cutDataSize);
    if (header == NULL)
        return NULL;

    uint64_t cutSizeTmp = *cutDataSize;
    uint64_t expectedSizeTmp = *expectedDataSize;

    if (bodyMustBe(header->assignment))
    {
        if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
            header->identification = createIdentification(data, size, error, expectedDataSize, cutDataSize);
        else if (header->assignment == Header::HEADER_ASSIGNMENT_STATE)
            header->state = createState(data, size, error, expectedDataSize, cutDataSize);
        else if (header->assignment == Header::HEADER_ASSIGNMENT_READ || header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
            header->readWrite = createReadWrite(data, size, error, expectedDataSize, cutDataSize);

        *cutDataSize += cutSizeTmp;
        *expectedDataSize += expectedSizeTmp;
    }

    return header;
}

struct Header* createHeader(uint8_t* data, uint64_t size, bool *error,
                            uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    *error = false;
    *expectedDataSize = 0;
    *cutDataSize = 0;

    if (size < HEADER_SIZE)
    {
        *expectedDataSize = HEADER_SIZE;
        return NULL;
    }

    uint64_t bodySize = 0;
    memcpy(&bodySize, &data[4], 8); // Размер тела пакета (документация)
    if (isLittleEndian())
        dataReverse(&bodySize, sizeof(bodySize));

    if (size < (HEADER_SIZE + bodySize))
    {
        *expectedDataSize = HEADER_SIZE + bodySize;
        return NULL;
    }

    uint64_t sum = data[0] + data[1] + data[2] + data[3] + bodySize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[12], 8); // Размер контрольной суммы пакета (документация)
    if (isLittleEndian())
        dataReverse(&chSum, sizeof(chSum));

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    struct Header *header = (struct Header *)malloc(sizeof(struct Header));
    if (header == NULL)
    {
        *error = true;
        return header;
    }

    header->version = data[0];
    header->type = static_cast<Header::Header_TYPE>(data[1]);
    header->assignment = static_cast<Header::Header_ASSIGNMENT>(data[2]);
    header->flags = static_cast<Header::Header_FLAGS>(data[3]);
    header->dataSize = bodySize;
    header->identification = NULL;
    header->readWrite = NULL;
    header->state = NULL;

    *cutDataSize = HEADER_SIZE;

    return header;
};

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

struct Identification* createIdentification(uint8_t * const data, uint64_t dataSize, bool *error,
                                            uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    *error = false;
    *expectedDataSize = 0;
    *cutDataSize = 0;

    if (dataSize < IDENTIFICATION_SIZE)
    {
        *expectedDataSize = IDENTIFICATION_SIZE;
        return NULL;
    }

    uint16_t id = (uint16_t)(data[0] << 8) | data[1];
    uint8_t nameSize = data[2];
    uint16_t descriptionSize = (uint16_t)(data[3] << 8) | data[4];
    uint8_t numberWriteChannel = data[5];
    uint8_t numberReadChannel = data[6];
    uint8_t flags = data[7];

    uint64_t sum = id + nameSize + descriptionSize + numberWriteChannel + numberReadChannel + flags;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[8], 8); // Размер контрольной суммы пакета (документация)
    if (isLittleEndian())
        dataReverse(&chSum, sizeof(chSum));

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    if (dataSize < (IDENTIFICATION_SIZE + nameSize + descriptionSize + numberWriteChannel + numberReadChannel))
    {
        *expectedDataSize = IDENTIFICATION_SIZE + nameSize + descriptionSize + numberWriteChannel + numberReadChannel;
        return NULL;
    }

    struct Identification *identificationResult = (struct Identification *)malloc(sizeof(struct Identification));
    if (identificationResult == NULL)
    {
        *error = true;
        return NULL;
    }

    struct Identification identification = {
        .flags = static_cast<Identification::Identification_FLAGS>(flags),
        .id = id,
        .nameSize = nameSize,
        .descriptionSize = descriptionSize,
        .numberWriteChannel= numberWriteChannel,
        .numberReadChannel= numberReadChannel,
        .name = (const char*)malloc(nameSize),
        .description = (const char*)malloc(descriptionSize),
        .writeChannelType = (const uint8_t *)malloc(numberWriteChannel),
        .readChannelType = (const uint8_t *)malloc(numberReadChannel),
    };

    memcpy((void *)identification.name, &data[IDENTIFICATION_SIZE], nameSize);
    memcpy((void *)identification.description, &data[IDENTIFICATION_SIZE + nameSize], descriptionSize);
    memcpy((void *)identification.writeChannelType, &data[IDENTIFICATION_SIZE + nameSize + descriptionSize], numberWriteChannel);
    memcpy((void *)identification.readChannelType, &data[IDENTIFICATION_SIZE + nameSize + descriptionSize + numberWriteChannel], numberReadChannel);

    memmove(identificationResult, &identification, sizeof(struct Identification));

    *cutDataSize = identificationSize(identificationResult);

    return identificationResult;
}

struct State *createState(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    *error = false;
    *expectedDataSize = 0;
    *cutDataSize = 0;

    if (size < STATE_SIZE)
    {
        *expectedDataSize = STATE_SIZE;
        return NULL;
    }

    uint8_t nameSize = data[0];
    uint8_t devState = data[1];
    uint8_t flags = data[2];

    uint64_t dataSize = 0;
    memcpy(&dataSize, &data[3], 4); // Размер данных пакета (документация)
    if (isLittleEndian())
        dataReverse(&dataSize, sizeof(dataSize));

    if (size < (STATE_SIZE + nameSize + dataSize))
    {
        *expectedDataSize = STATE_SIZE + nameSize + dataSize;
        return NULL;
    }

    uint64_t sum = nameSize + devState + flags + dataSize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[7], 8); // Размер контрольной суммы пакета (документация)
    if (isLittleEndian())
        dataReverse(&chSum, sizeof(chSum));

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    struct State *state = (struct State *)malloc(sizeof(struct State));
    if (state == NULL)
    {
        *error = true;
        return NULL;
    }

    state->nameSize = nameSize;
    state->state = static_cast<State::State_STATE>(devState);
    state->flags = static_cast<State::State_FLAGS>(flags);
    state->dataSize = dataSize;

    state->name = (char *)malloc(nameSize);
    memcpy(state->name, &data[STATE_SIZE], nameSize);

    state->data = (uint8_t *)malloc(dataSize);
    memcpy(state->data, &data[STATE_SIZE + nameSize], dataSize);

    *cutDataSize = stateSize(state);

    return state;
}

struct Read_Write *createReadWrite(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    *error = false;
    *expectedDataSize = 0;
    *cutDataSize = 0;

    if (size < READ_WRITE_SIZE)
    {
        *expectedDataSize = READ_WRITE_SIZE;
        return NULL;
    }

    uint8_t nameSize = data[0];
    uint8_t channelNumber = data[1];
    uint8_t flags = data[2];

    uint64_t dataSize = 0;
    memcpy(&dataSize, &data[3], 4); // Размер данных пакета (документация)
    if (isLittleEndian())
        dataReverse(&dataSize, sizeof(dataSize));

    if (size < (READ_WRITE_SIZE + nameSize + dataSize))
    {
        *expectedDataSize = READ_WRITE_SIZE + nameSize + dataSize;
        return NULL;
    }

    uint64_t sum = nameSize + channelNumber + flags + dataSize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[7], 8); // Размер контрольной суммы пакета (документация)
    if (isLittleEndian())
        dataReverse(&chSum, sizeof(chSum));

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    struct Read_Write *readWriteReslut = (struct Read_Write *)malloc(sizeof(struct Read_Write));
    if (readWriteReslut == NULL)
    {
        *error = true;
        return NULL;
    }

    struct Read_Write readWrite = {
        .flags = static_cast<Read_Write::ReadWrite_FLAGS>(flags),
        .nameSize = nameSize,
        .channelNumber = channelNumber,
        .dataSize = dataSize,
        .name = (char *)malloc(nameSize),   //!! не обрабатывается случай, если память не выделилась
        .data = (uint8_t *)malloc(dataSize) //!! не обрабатывается случай, если память не выделилась
    };

    memcpy(&readWrite.name, &data[READ_WRITE_SIZE], nameSize);
    memcpy(&readWrite.data, &data[READ_WRITE_SIZE + nameSize], dataSize);

    memmove(readWriteReslut, &readWrite, sizeof(struct Read_Write));
    *cutDataSize = readWriteSize(readWriteReslut);

    return readWriteReslut;
}

bool bodyMustBe(uint8_t assigment)
{
    switch (assigment)
    {
    case Header::HEADER_ASSIGNMENT_IDENTIFICATION :
    case Header::HEADER_ASSIGNMENT_STATE :
    case Header::HEADER_ASSIGNMENT_READ :
    case Header::HEADER_ASSIGNMENT_WRITE :
        return true;
    }

    return false;
}

