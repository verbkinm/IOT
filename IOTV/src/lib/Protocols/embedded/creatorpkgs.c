#include "creatorpkgs.h"
#include "iotv_types.h"

#include <stdlib.h>
#include <string.h>


header_t* createPkgs(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    header_t *header = createHeader(data, size, error, expectedDataSize, cutDataSize);

    if (header == NULL)
        return NULL;

    if (isBodyMustBe(header->type, header->assignment))
    {
        if (header->dataSize == 0)
        {
            // В случае с пустым списком устройств
            if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
                return header;

            *error = true;
            return header;
        }

        if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
            header->pkg = createIdentification(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == HEADER_ASSIGNMENT_STATE)
            header->pkg = createState(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == HEADER_ASSIGNMENT_READ || header->assignment ==HEADER_ASSIGNMENT_WRITE)
            header->pkg = createReadWrite(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == HEADER_ASSIGNMENT_TECH)
            header->pkg = createTech(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == HEADER_ASSIGNMENT_HOST_BROADCAST)
            header->pkg = createHostBroadCast(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);
        else if (header->assignment == HEADER_ASSIGNMENT_LOG_DATA)
            header->pkg = createLogData(&data[HEADER_SIZE], size - *cutDataSize, error, expectedDataSize, cutDataSize);

        // Если cutDataSize > 0, то пакет body сформирован
        // Но если он НЕ равен тому, что ожидает header->dataSize, то это ошибка
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

header_t* createHeader(uint8_t *data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
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

    uint64_t bodySize;
    memcpy(&bodySize, &data[8], 8); // 8 - размер тела пакета (документация)

    if (size < (HEADER_SIZE + bodySize))
    {
        *expectedDataSize = HEADER_SIZE + bodySize;
        return NULL;
    }

    uint16_t fragmentNumber;
    memcpy(&fragmentNumber, &data[4], 2);

    uint16_t numberOfFragments;
    memcpy(&numberOfFragments, &data[6], 2);

    if (fragmentNumber > numberOfFragments)
    {
        *error = true;
        return NULL;
    }


    uint64_t sum = data[0] + data[1] + data[2] + data[3] + fragmentNumber + numberOfFragments + bodySize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[16], 8); // 8 - размер контрольной суммы пакета (документация)

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    header_t *headerResult = (header_t *)malloc(sizeof(header_t));

    if (headerResult == NULL)
    {
        *error = true;
        return headerResult;
    }

    headerResult->version = data[0];
    headerResult->type = data[1];
    headerResult->assignment = data[2];
    headerResult->flags = data[3];
    headerResult->fragment = fragmentNumber;
    headerResult->fragments = numberOfFragments;
    headerResult->dataSize = bodySize;
    headerResult->pkg = NULL;

    *cutDataSize = HEADER_SIZE;

    return headerResult;
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

struct Identification *createIdentification(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    *error = false;
    *expectedDataSize = 0;
    *cutDataSize = 0;

    if (size < IDENTIFICATION_SIZE)
    {
        *expectedDataSize = IDENTIFICATION_SIZE;
        return NULL;
    }

    uint16_t id = (uint16_t)(data[0] << 8) | data[1];
    uint8_t nameSize = data[2];
    uint8_t numberWriteChannel = data[3];
    uint8_t numberReadChannel = data[4];
    uint16_t descriptionSize = (uint16_t)(data[5] << 8) | data[6];

    uint8_t flags = data[7];

    uint64_t sum = id + nameSize + descriptionSize + numberWriteChannel + numberReadChannel + flags;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[8], 8); // Размер контрольной суммы пакета (документация)

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    if (size < (uint64_t)(IDENTIFICATION_SIZE + nameSize + descriptionSize + numberWriteChannel + numberReadChannel))
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

    identificationResult->id = id;
    identificationResult->descriptionSize = descriptionSize;
    identificationResult->nameSize = nameSize;
    identificationResult->numberWriteChannel= numberWriteChannel;
    identificationResult->numberReadChannel= numberReadChannel;
    identificationResult->flags = flags;
    identificationResult->name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL;
    identificationResult->description = (descriptionSize > 0) ? (char *)malloc(descriptionSize) : NULL;
    identificationResult->writeChannelType = (numberWriteChannel > 0) ? (uint8_t *)malloc(numberWriteChannel) : NULL;
    identificationResult->readChannelType = (numberReadChannel > 0) ? (uint8_t *)malloc(numberReadChannel) : NULL;

    if ((nameSize > 0 && identificationResult->name == NULL)
        || (descriptionSize > 0 && identificationResult->description == NULL)
        || (numberReadChannel > 0 && identificationResult->readChannelType == NULL)
        || (numberWriteChannel > 0 && identificationResult->writeChannelType == NULL))
    {
        *error = true;
        clearIdentification(identificationResult);
        return NULL;
    }

    if (nameSize > 0)
        memcpy((void *)identificationResult->name, &data[IDENTIFICATION_SIZE], nameSize);
    if (descriptionSize > 0)
        memcpy((void *)identificationResult->description, &data[IDENTIFICATION_SIZE + nameSize], descriptionSize);
    if (numberWriteChannel > 0)
        memcpy((void *)identificationResult->writeChannelType, &data[IDENTIFICATION_SIZE + nameSize + descriptionSize], numberWriteChannel);
    if (numberReadChannel > 0)
        memcpy((void *)identificationResult->readChannelType, &data[IDENTIFICATION_SIZE + nameSize + descriptionSize + numberWriteChannel], numberReadChannel);

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

    stateResult->nameSize = nameSize;
    stateResult->state = devState;
    stateResult->flags = flags;
    stateResult->dataSize = dataSize;
    stateResult->name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL;
    stateResult->data = (dataSize > 0) ? (uint8_t *)malloc(dataSize) : NULL;

    if ( ((nameSize > 0) && (stateResult->name == NULL)) || ((dataSize > 0) && (stateResult->data == NULL)) )
    {
        *error = true;
        clearState(stateResult);
        return NULL;
    }

    if (nameSize > 0)
        memcpy((void *)stateResult->name, &data[STATE_SIZE], nameSize);
    if (dataSize > 0)
        memcpy((void *)stateResult->data, &data[STATE_SIZE + nameSize], dataSize);

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
    memcpy(&dataSize, &data[3], 4); // 4 - размер данных пакета (документация)

    if (size < (READ_WRITE_SIZE + nameSize + dataSize))
    {
        *expectedDataSize = READ_WRITE_SIZE + nameSize + dataSize;
        return NULL;
    }

    uint64_t sum = nameSize + channelNumber + flags + dataSize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[7], 8); // 8 - размер контрольной суммы пакета (документация)

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

    readWriteReslut->nameSize = nameSize;
    readWriteReslut->channelNumber = channelNumber;
    readWriteReslut->flags = flags;
    readWriteReslut->dataSize = dataSize;
    readWriteReslut->name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL;
    readWriteReslut->data = NULL;

    if (dataSize > 0 && (dataSize <= (size - READ_WRITE_SIZE + nameSize)))
        readWriteReslut->data = (char *)malloc(dataSize);

    // Память не была выделена
    if ( ((nameSize > 0) && (readWriteReslut->name == NULL)) || ((dataSize > 0) && (readWriteReslut->data == NULL)) )
    {
        *error = true;
        clearReadWrite(readWriteReslut);
        return NULL;
    }

    if (nameSize > 0)
        memcpy((void *)readWriteReslut->name, &data[READ_WRITE_SIZE], nameSize);
    if (dataSize > 0)
        memcpy((void *)readWriteReslut->data, &data[READ_WRITE_SIZE + nameSize], dataSize);

    *cutDataSize = readWriteSize(readWriteReslut);

    return readWriteReslut;
}

struct Tech *createTech(uint8_t * data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    *error = false;
    *expectedDataSize = 0;
    *cutDataSize = 0;

    if (size < TECH_SIZE)
    {
        *expectedDataSize = TECH_SIZE;
        return NULL;
    }

    uint8_t type = data[0];
    //    uint8_t devState = data[1];
    uint8_t flags = data[2];

    uint32_t dataSize = 0;
    memcpy(&dataSize, &data[3], 4); // 4 - Размер данных пакета (документация)

    if (size < (TECH_SIZE + dataSize))
    {
        *expectedDataSize = TECH_SIZE + dataSize;
        return NULL;
    }

    uint64_t sum = type + flags + dataSize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[7], 8); // 8 - Размер контрольной суммы пакета (документация)

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    struct Tech *techResult = (struct Tech *)malloc(sizeof(struct Tech));
    if (techResult == NULL)
    {
        *error = true;
        return NULL;
    }

    techResult->flags = flags;
    techResult->type = type;
    techResult->dataSize = dataSize;
    techResult->data = (dataSize > 0) ? (uint8_t *)malloc(dataSize) : NULL;

    if ((dataSize > 0) && (techResult->data == NULL))
    {
        *error = true;
        clearTech(techResult);
        return NULL;
    }

    if (dataSize > 0)
        memcpy((void *)techResult->data, &data[TECH_SIZE], dataSize);

    *cutDataSize = techSize(techResult);

    return techResult;
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
        case HEADER_ASSIGNMENT_TECH :
        case HEADER_ASSIGNMENT_HOST_BROADCAST :
        case HEADER_ASSIGNMENT_LOG_DATA :
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
        case HEADER_ASSIGNMENT_TECH :
        case HEADER_ASSIGNMENT_LOG_DATA :
            return true;
        }
    }

    return false;
}

struct Host_Broadcast *createHostBroadCast(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    *error = false;
    *expectedDataSize = 0;
    *cutDataSize = 0;

    if (size < HOST_BROADCAST_SIZE)
    {
        *expectedDataSize = HOST_BROADCAST_SIZE;
        return NULL;
    }

    uint8_t nameSize = data[0];

    if (size < (uint64_t)(HOST_BROADCAST_SIZE + nameSize))
    {
        *expectedDataSize = HOST_BROADCAST_SIZE + nameSize;
        return NULL;
    }

    uint32_t address = 0;
    memcpy(&address, &data[1], 4); // 4 - документация

    uint8_t flags = data[5];

    uint16_t port = 0;
    memcpy(&port, &data[6], 2); // 2 - документация

    uint64_t sum = nameSize + address + flags + port;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[8], 8); // Размер контрольной суммы пакета (документация)

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    struct Host_Broadcast *hostBroadcastResult = (struct Host_Broadcast *)malloc(sizeof(struct Host_Broadcast));
    if (hostBroadcastResult == NULL)
    {
        *error = true;
        return NULL;
    }

    hostBroadcastResult->nameSize = nameSize;
    hostBroadcastResult->address = address;
    hostBroadcastResult->flags = flags;
    hostBroadcastResult->port = port;
    hostBroadcastResult->name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL;

    if ((nameSize > 0) && (hostBroadcastResult->name == NULL))
    {
        *error = true;
        clearHostBroadCast(hostBroadcastResult);
        return NULL;
    }

    if (nameSize > 0)
        memcpy((void *)hostBroadcastResult->name, &data[HOST_BROADCAST_SIZE], nameSize);

    *cutDataSize = hostBroadCastSize(hostBroadcastResult);

    return hostBroadcastResult;
}

struct Log_Data *createLogData(uint8_t * const data, uint64_t size, bool *error, uint64_t *expectedDataSize, uint64_t *cutDataSize)
{
    if ((data == NULL ) || (error == NULL) || (expectedDataSize == NULL) || (cutDataSize == NULL) )
        return NULL;

    *error = false;
    *expectedDataSize = 0;
    *cutDataSize = 0;

    if (size < LOG_DATA_SIZE)
    {
        *expectedDataSize = LOG_DATA_SIZE;
        return NULL;
    }

    uint8_t nameSize = data[0];

    uint64_t startInterval =  0;
    memcpy((void *)&startInterval, &data[1], 8); // 8 - документация

    uint64_t endInterval =  0;
    memcpy((void *)&endInterval, &data[9], 8); // 8 - документация

    uint32_t interval =  0;
    memcpy((void *)&interval, &data[17], 4); // 4 - документация

    uint8_t flags = data[21];
    uint8_t channelNumber = data[22];

    uint32_t dataSize = 0;
    memcpy(&dataSize, &data[23], 4); // 4 - размер данных пакета (документация)

    if (size < (LOG_DATA_SIZE + nameSize + dataSize))
    {
        *expectedDataSize = LOG_DATA_SIZE + nameSize + dataSize;
        return NULL;
    }

    uint64_t sum = nameSize + startInterval + endInterval + interval + flags + channelNumber + dataSize;
    uint64_t chSum = 0;
    memcpy(&chSum, &data[27], 8); // 8 - размер контрольной суммы пакета (документация)

    if (sum != chSum)
    {
        *error = true;
        return NULL;
    }

    struct Log_Data *logDataReslut = (struct Log_Data *)malloc(sizeof(struct Log_Data));

    if (logDataReslut == NULL)
    {
        *error = true;
        return NULL;
    }

    logDataReslut->nameSize = nameSize;
    logDataReslut->startInterval = startInterval;
    logDataReslut->endInterval = endInterval;
    logDataReslut->interval = interval;
    logDataReslut->flags = flags;
    logDataReslut->channelNumber = channelNumber;
    logDataReslut->dataSize = dataSize;
    logDataReslut->name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL;
    logDataReslut->data = NULL;

    if (dataSize > 0 && (dataSize <= (size - LOG_DATA_SIZE + nameSize)))
        logDataReslut->data = (char *)malloc(dataSize);

    // Память не была выделена
    if ( ((nameSize > 0) && (logDataReslut->name == NULL)) || ((dataSize > 0) && (logDataReslut->data == NULL)) )
    {
        *error = true;
        clearLogData(logDataReslut);
        return NULL;
    }

    if (nameSize > 0)
        memcpy((void *)logDataReslut->name, &data[LOG_DATA_SIZE], nameSize);
    if (dataSize > 0)
        memcpy((void *)logDataReslut->data, &data[LOG_DATA_SIZE + nameSize], dataSize);

    *cutDataSize = logDataSize(logDataReslut);

    return logDataReslut;
}
