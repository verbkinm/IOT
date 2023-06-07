#include "creatorpkgs.h"

#include "string.h"
struct Header* createPkgs(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    struct Header *header = createHeader(data, size, error, expectedDataSize, cutDataSize);
    if (header == NULL)
        return NULL;

    if (isBodyMustBe(header->type, header->assignment))
    {
        if (header->dataSize == 0)
        {
            *error = true;
            return header;
        }

        if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
            header->identification = createIdentification(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == HEADER_ASSIGNMENT_STATE)
            header->state = createState(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == HEADER_ASSIGNMENT_READ || header->assignment == HEADER_ASSIGNMENT_WRITE)
            header->readWrite = createReadWrite(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);

        // Если cutDataSize > 0, то пакет body сформирован
        // Но если он не равен тому, что ожидает header->dataSize, то это ошибка
        if ( (*error == true) || (*cutDataSize != header->dataSize) )
        {
            *cutDataSize = 0;
            *expectedDataSize = 0;
            return header;
        }

        *cutDataSize += HEADER_SIZE;
    }

    return header;
}

struct Header* createHeader(uint8_t *data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
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

    if (size < (HEADER_SIZE + bodySize))
    {
        *expectedDataSize = HEADER_SIZE + bodySize;
        return NULL;
    }

    uint64_t sum = data[0] + data[1] + data[2] + data[3] + bodySize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[12], 8); // Размер контрольной суммы пакета (документация)

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
        .type = (Header_TYPE)data[1],
        .assignment = (Header_ASSIGNMENT)data[2],
        .flags = (Header_FLAGS)data[3],
        .version = data[0],
        .dataSize = bodySize,
        .identification = NULL,
        .readWrite = NULL,
        .state = NULL
    };

    memcpy((void *)headerResult, &header, sizeof(struct Header));
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

struct Identification *createIdentification(uint8_t * const data, uint64_t dataSize, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
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

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    if (dataSize < (uint64_t)(IDENTIFICATION_SIZE + nameSize + descriptionSize + numberWriteChannel + numberReadChannel))
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
        .flags = (Identification_FLAGS)flags,
        .id = id,
        .nameSize = nameSize,
        .descriptionSize = descriptionSize,
        .numberWriteChannel= numberWriteChannel,
        .numberReadChannel= numberReadChannel,
        .name = (nameSize > 0) ? (const char*)malloc(nameSize) : NULL,
        .description = (descriptionSize > 0) ? (const char*)malloc(descriptionSize) : NULL,
        .writeChannelType = (numberWriteChannel > 0) ? (const uint8_t *)malloc(numberWriteChannel) : NULL,
        .readChannelType = (numberReadChannel > 0) ? (const uint8_t *)malloc(numberReadChannel) : NULL,
    };

    if (nameSize > 0)
        memcpy((void *)identification.name, &data[IDENTIFICATION_SIZE], nameSize);
    if (descriptionSize > 0)
        memcpy((void *)identification.description, &data[IDENTIFICATION_SIZE + nameSize], descriptionSize);
    if (numberWriteChannel > 0)
        memcpy((void *)identification.writeChannelType, &data[IDENTIFICATION_SIZE + nameSize + descriptionSize], numberWriteChannel);
    if (numberReadChannel > 0)
        memcpy((void *)identification.readChannelType, &data[IDENTIFICATION_SIZE + nameSize + descriptionSize + numberWriteChannel], numberReadChannel);

    memcpy((void *)identificationResult, &identification, sizeof(struct Identification));

    if ((nameSize > 0 && identification.name == NULL)
            || (descriptionSize > 0 && identification.description == NULL)
            || (numberReadChannel > 0 && identification.readChannelType == NULL)
            || (numberWriteChannel > 0 && identification.writeChannelType == NULL))
    {
        *error = true;
        clearIdentification(identificationResult);
        return NULL;
    }

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

    uint32_t dataSize = 0;
    memcpy(&dataSize, &data[3], 4); // Размер данных пакета (документация)

    if (size < (STATE_SIZE + nameSize + dataSize))
    {
        *expectedDataSize = STATE_SIZE + nameSize + dataSize;
        return NULL;
    }

    uint64_t sum = nameSize + devState + flags + dataSize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[7], 8); // Размер контрольной суммы пакета (документация)

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
        .flags = (State_FLAGS)flags,
        .state = (State_STATE)devState,
        .nameSize = nameSize,
        .dataSize = dataSize,
        .name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL,
        .data = (dataSize > 0) ? (uint8_t *)malloc(dataSize) : NULL
    };

    if (nameSize > 0)
        memcpy((void *)state.name, &data[STATE_SIZE], nameSize);
    if (dataSize > 0)
        memcpy((void *)state.data, &data[STATE_SIZE + nameSize], dataSize);

    memcpy((void *)stateResult, &state, sizeof(struct State));

    if ( ((nameSize > 0) && (stateResult->name == NULL)) || ((dataSize > 0) && (stateResult->data == NULL)) )
    {
        *error = true;
        clearState(stateResult);
        return NULL;
    }

    *cutDataSize = stateSize(stateResult);

    return stateResult;
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

    uint32_t dataSize = 0;
    memcpy(&dataSize, &data[3], 4); // Размер данных пакета (документация)

    if (size < (READ_WRITE_SIZE + nameSize + dataSize))
    {
        *expectedDataSize = READ_WRITE_SIZE + nameSize + dataSize;
        return NULL;
    }

    uint64_t sum = nameSize + channelNumber + flags + dataSize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[7], 8); // Размер контрольной суммы пакета (документация)

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
        .flags = (ReadWrite_FLAGS)flags,
        .nameSize = nameSize,
        .channelNumber = channelNumber,
        .dataSize = dataSize,
        .name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL,
        .data = (dataSize > 0) ? (char *)malloc(dataSize) : NULL
    };

    if (nameSize > 0)
        memcpy((void *)readWrite.name, &data[READ_WRITE_SIZE], nameSize);
    if (dataSize > 0)
        memcpy((void *)readWrite.data, &data[READ_WRITE_SIZE + nameSize], dataSize);

    memcpy((void *)readWriteReslut, &readWrite, sizeof(struct Read_Write));

    if ( ((nameSize > 0) && (readWriteReslut->name == NULL)) || ((dataSize > 0) && (readWriteReslut->data == NULL)) )
    {
        *error = true;
        clearReadWrite(readWriteReslut);
        return NULL;
    }

    *cutDataSize = readWriteSize(readWriteReslut);

    return readWriteReslut;
}

bool isBodyMustBe(uint8_t type, uint8_t assigment)
{
    if (type == HEADER_TYPE_RESPONSE)
    {
        switch (assigment)
        {
            case HEADER_ASSIGNMENT_IDENTIFICATION :
            case HEADER_ASSIGNMENT_STATE :
            case HEADER_ASSIGNMENT_READ :
            case HEADER_ASSIGNMENT_WRITE :
            return true;
        }
    }
    else if (type == HEADER_TYPE_REQUEST)
    {
        switch (assigment)
        {
            case HEADER_ASSIGNMENT_STATE :
            case HEADER_ASSIGNMENT_READ :
            case HEADER_ASSIGNMENT_WRITE :
            return true;
        }
    }

    return false;
}

