#include "creatorpkgs.h"

struct Header* createPkgs(uint8_t* const data, uint64_t size, bool *error,
                          uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    struct Header *header = createHeader(data, size, error, expectedDataSize, cutDataSize);
    if (header == NULL)
        return NULL;

    if (bodyMustBe(header->assignment))
    {
        if (header->dataSize == 0)
        {
            *error = true;
            return header;
        }

        if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
            header->identification = createIdentification(data, size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == Header::HEADER_ASSIGNMENT_STATE)
            header->state = createState(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == Header::HEADER_ASSIGNMENT_READ || header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
            header->readWrite = createReadWrite(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);

        if (*cutDataSize > 0)
        {
            if (*cutDataSize != header->dataSize)
            {
                *error = true;
                return header;
            }
            *cutDataSize += HEADER_SIZE;
        }
        *expectedDataSize += HEADER_SIZE;
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

    struct Header *headerResult = (struct Header *)malloc(sizeof(struct Header));
    if (headerResult == NULL)
    {
        *error = true;
        return headerResult;
    }

    struct Header header = {
        .type = static_cast<Header::Header_TYPE>(data[1]),
        .assignment = static_cast<Header::Header_ASSIGNMENT>(data[2]),
        .flags = static_cast<Header::Header_FLAGS>(data[3]),
        .version = data[0],
        .dataSize = bodySize,
        .identification = NULL,
        .readWrite = NULL,
        .state = NULL
    };

    memmove(headerResult, &header, sizeof(Header));
    *cutDataSize = HEADER_SIZE;

    return headerResult;
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
        .name = (nameSize > 0) ? (const char*)malloc(nameSize) : NULL,                              //!! не обрабатывается случай, если память не выделилась
        .description = (descriptionSize > 0) ? (const char*)malloc(descriptionSize) : NULL,                //!! не обрабатывается случай, если память не выделилась
        .writeChannelType = (numberWriteChannel > 0) ? (const uint8_t *)malloc(numberWriteChannel) : NULL,    //!! не обрабатывается случай, если память не выделилась
        .readChannelType = (numberReadChannel > 0) ? (const uint8_t *)malloc(numberReadChannel) : NULL,      //!! не обрабатывается случай, если память не выделилась
    };

    if (nameSize > 0)
        memcpy((void *)identification.name, &data[IDENTIFICATION_SIZE], nameSize);
    if (descriptionSize > 0)
        memcpy((void *)identification.description, &data[IDENTIFICATION_SIZE + nameSize], descriptionSize);
    if (numberWriteChannel > 0)
        memcpy((void *)identification.writeChannelType, &data[IDENTIFICATION_SIZE + nameSize + descriptionSize], numberWriteChannel);
    if (numberReadChannel > 0)
        memcpy((void *)identification.readChannelType, &data[IDENTIFICATION_SIZE + nameSize + descriptionSize + numberWriteChannel], numberReadChannel);

    memmove(identificationResult, &identification, sizeof(struct Identification));

    *cutDataSize = identificationSize(identificationResult);

    return identificationResult;
}

struct State *createState(uint8_t * const data, uint64_t size, bool *error,
                          uint64_t *expectedDataSize, uint64_t *cutDataSize)
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

    struct State *stateResult = (struct State *)malloc(sizeof(struct State));
    if (stateResult == NULL)
    {
        *error = true;
        return NULL;
    }

    struct State state = {
        .flags = static_cast<State::State_FLAGS>(flags),
        .state = static_cast<State::State_STATE>(devState),
        .nameSize = nameSize,
        .dataSize = dataSize,
        .name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL,   //!! не обрабатывается случай, если память не выделилась
        .data = (dataSize > 0) ? (uint8_t *)malloc(dataSize) : NULL //!! не обрабатывается случай, если память не выделилась
    };

    if (nameSize > 0)
        memcpy(&state.name, &data[STATE_SIZE], nameSize);
    if (dataSize > 0)
        memcpy(&state.data, &data[STATE_SIZE + nameSize], dataSize);

    memmove(stateResult, &state, sizeof(struct State));
    *cutDataSize = stateSize(stateResult);

    return stateResult;
}

struct Read_Write *createReadWrite(uint8_t * const data, uint64_t size, bool *error,
                                   uint64_t *expectedDataSize, uint64_t *cutDataSize)
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
        .name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL,   //!! не обрабатывается случай, если память не выделилась
        .data = (dataSize > 0) ? (uint8_t *)malloc(dataSize) : NULL //!! не обрабатывается случай, если память не выделилась
    };

    if (nameSize > 0)
        memcpy(&readWrite.name, &data[READ_WRITE_SIZE], nameSize);
    if (dataSize > 0)
        memcpy(&readWrite.data, &data[READ_WRITE_SIZE + nameSize], dataSize);

    memmove(readWriteReslut, &readWrite, sizeof(struct Read_Write));
    *cutDataSize = readWriteSize(readWriteReslut);

    return readWriteReslut;
}

bool bodyMustBe(uint8_t assigment)
{
    switch (assigment)
    {
//    case Header::HEADER_ASSIGNMENT_IDENTIFICATION :
        case Header::HEADER_ASSIGNMENT_STATE :
        case Header::HEADER_ASSIGNMENT_READ :
        case Header::HEADER_ASSIGNMENT_WRITE :
            return true;
    }

    return false;
}

