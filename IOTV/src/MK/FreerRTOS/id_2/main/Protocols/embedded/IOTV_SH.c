#include "IOTV_SH.h"
#include "identification.h"
#include "read_write.h"
#include "state.h"
#include "tech.h"
#include "log_data.h"
#include "iotv_types.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static uint64_t responceReadWritePkgCount(uint64_t dataOutSize, const iotv_obj_t *iot, const header_t *header);

uint64_t responseIdentificationData(char *outData, uint64_t outDataSize, const iotv_obj_t *iot,
                                    uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, identification_flag_t ident_flags, header_flag_t header_flags)
{
    if (outData == NULL || writeFunc == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    if (iot == NULL)
    {
        header_t header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
            .flags = HEADER_FLAGS_NONE,
            .fragment = 1,
            .fragments = 1,
            .dataSize = 0,
            .pkg = NULL
        };

        uint64_t resultSize =  headerToData(&header, outData, outDataSize);
        return writeFunc(outData, resultSize, obj);
    }

    uint64_t pkgsCount;
    uint64_t rawDataSize;

    rawDataSize = iot->descriptionSize;

    pkgsCount = pkgCount(rawDataSize, outDataSize, HEADER_SIZE + IDENTIFICATION_SIZE + iot->nameSize + iot->numberWriteChannel + iot->numberReadChannel);

    // Максимальное значение uint16_t = 0xFFFF. fragments в header не может привышать данное значение!
    if (pkgsCount > (uint64_t)0xFFFF)
    {
        RETURN_WARNING;
        return 0;
    }

    char *rawData = iot->description;

    char *it = rawData;
    char *end = rawData + rawDataSize;
    uint64_t totalSendByte = 0;

    for (uint16_t i = 0; i < pkgsCount; ++i)
    {
        uint64_t rawDataPart = outDataSize - HEADER_SIZE - IDENTIFICATION_SIZE - iot->nameSize - iot->numberWriteChannel - iot->numberReadChannel;
        uint64_t sizeLeft = end - it;
        if (sizeLeft < rawDataPart)
            rawDataPart = sizeLeft;

        identification_t ident = {
            .id = iot->id,
            .descriptionSize = rawDataPart,
            .nameSize = iot->nameSize,
            .numberWriteChannel = iot->numberWriteChannel,
            .numberReadChannel = iot->numberReadChannel,
            .flags = ident_flags,
            .name = iot->name,
            .description = it,
            .writeChannelType = iot->writeChannelType,
            .readChannelType = iot->readChannelType,
        };

        header_t header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
            .flags = header_flags,
            .fragment = i + 1,
            .fragments = (uint16_t)(pkgsCount),
            .dataSize = identificationSize(&ident),
            .pkg = &ident
        };

        it += rawDataPart;

        uint64_t resultSize = headerToData(&header, outData, outDataSize);
        totalSendByte += writeFunc(outData, resultSize, obj);
    }

    return totalSendByte;
}

uint64_t responsePingData(char *outData, uint64_t outDataSize,
                          uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, header_flag_t header_flags)
{
    if (outData == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_PING_PONG,
        .flags = header_flags,
        .fragment = 1,
        .fragments = 1,
        .dataSize = 0,
        .pkg = NULL
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

uint64_t responseReadData(char *outData, uint64_t outDataSize, const iotv_obj_t *iot, const header_t *head,
                          uint64_t (*writeFunc)(char *, uint64_t, void *obj), void *obj, readwrite_flag_t rw_flags, header_flag_t header_flags)
{
    if (outData == NULL || iot == NULL || head == NULL || writeFunc == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    read_write_t *rwPkg = ((read_write_t *)head->pkg);
    uint64_t pkgsCount;
    uint64_t totalSendByte = 0;

    if (rwPkg->flags == ReadWrite_FLAGS_IGNORE_CH)
    {
        for (int i = 0; i < iot->numberReadChannel; ++i)
        {
            rwPkg->flags = ReadWrite_FLAGS_NONE;
            rwPkg->channelNumber = i;

            totalSendByte += responseReadData(outData, outDataSize, iot, head, writeFunc, obj, 0, 0);
        }
        return totalSendByte;
    }

    if (rwPkg->flags != ReadWrite_FLAGS_OPEN_STREAM
        && (iot->readChannelType[rwPkg->channelNumber] == DATA_TYPE_RAW))
    {
        RETURN_WARNING;
        return 0;
    }

    pkgsCount = responceReadWritePkgCount(outDataSize, iot, head);

    // Максимальное значение uint16_t = 0xFFFF. fragments в header не может привышать данное значение!
    if (pkgsCount > (uint64_t)0xFFFF)
    {
        RETURN_WARNING;
        return 0;
    }

    for (uint16_t i = 0; i < pkgsCount; ++i)
    {
        char *it = NULL;
        uint64_t dataReadSize = dataPart(&it, i, outDataSize - (HEADER_SIZE + READ_WRITE_SIZE + iot->nameSize), iot, rwPkg->channelNumber);

        read_write_t readWrite = {
            .dataSize = dataReadSize,
            .nameSize = iot->nameSize,
            .channelNumber = rwPkg->channelNumber,
            .flags = rw_flags,
            .name = iot->name,
            .data = it
        };

        header_t header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_READ,
            .flags = header_flags,
            .fragment = (uint16_t)(i + 1),
            .fragments = (uint16_t)(pkgsCount),
            .dataSize = readWriteSize(&readWrite),
            .pkg = &readWrite
        };

        uint64_t resultSize = headerToData(&header, outData, outDataSize);
        totalSendByte += writeFunc(outData, resultSize, obj);
    }

    return totalSendByte;
}

uint64_t responseWriteData(char *outData, uint64_t outDataSize, iotv_obj_t *iot, const header_t *head,
                           uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, readwrite_flag_t rw_flags, header_flag_t header_flags)
{
    if (head == NULL || outData == NULL || iot == NULL || head->pkg == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    read_write_t *ptrReadWrite = ((read_write_t *)head->pkg);

    if (iot->readChannel == NULL
        || iot->numberWriteChannel <= ptrReadWrite->channelNumber
        || iot->numberReadChannel <= ptrReadWrite->channelNumber)
    {
        RETURN_WARNING;
        return 0;
    }

    // Непосредственно запись данных в iot объект
    if (ptrReadWrite->dataSize > 0)
    {
        if (iot->readChannel[ptrReadWrite->channelNumber].data != NULL
            && iot->readChannel[ptrReadWrite->channelNumber].dataSize == ptrReadWrite->dataSize)
        {
            memcpy(iot->readChannel[ptrReadWrite->channelNumber].data, ptrReadWrite->data, iot->readChannel[ptrReadWrite->channelNumber].dataSize);
        }
    }

    struct Read_Write readWrite = {
        .dataSize = 0,
        .nameSize = iot->nameSize,
        .channelNumber = ptrReadWrite->channelNumber,
        .flags = rw_flags,
        .name = iot->name,
        .data = NULL
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_WRITE,
        .flags = header_flags,
        .fragment = head->fragment,
        .fragments = head->fragments,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite,
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

uint64_t responseStateData(char *outData, uint64_t outDataSize, const iotv_obj_t *iot,
                           uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, state_flag_t rw_flags, header_flag_t header_flags)
{
    if (outData == NULL || iot == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    state_pkg_t state = {
        .nameSize = iot->nameSize,
        .state = iot->state,
        .flags = rw_flags,
        .dataSize = 0,
        .name = iot->name,
        .data = NULL
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_STATE,
        .flags = header_flags,
        .fragment = 1,
        .fragments = 1,
        .dataSize = stateSize(&state),
        .pkg = &state
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

uint64_t queryIdentificationData(char *outData, uint64_t outDataSize, uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, header_flag_t header_flags)
{
    if (outData == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_IDENTIFICATION,
        .flags = header_flags,
        .fragment = 1,
        .fragments = 1,
        .dataSize = 0,
        .pkg = NULL
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

uint64_t queryPingData(char *outData, uint64_t outDataSize, uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, header_flag_t header_flags)
{
    if (outData == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_PING_PONG,
        .flags = header_flags,
        .fragment = 1,
        .fragments = 1,
        .dataSize = 0,
        .pkg = NULL
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

uint64_t queryWriteData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, const char *dataToWrite, uint32_t dataWriteSize,
                        uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, readwrite_flag_t readwrite_flag, header_flag_t header_flags)
{
    if (outData == NULL || dataToWrite == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    read_write_t readWrite = {
        .dataSize = dataWriteSize,
        .nameSize = (uint8_t)strlen(name),
        .channelNumber = channelNumber,
        .flags = readwrite_flag,
        .name = (char *)name,
        .data = (char *)dataToWrite
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_WRITE,
        .flags = header_flags,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite,
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

uint64_t queryReadData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber,
                       uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, readwrite_flag_t readwrite_flag, header_flag_t header_flags)
{
    if (outData == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    read_write_t readWrite = {
        .dataSize = 0,
        .nameSize = (uint8_t)strlen(name),
        .channelNumber = channelNumber,
        .flags = readwrite_flag,
        .name = (char *)name,
        .data = NULL
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_READ,
        .flags = header_flags,
        .fragment = 1,
        .fragments = 1,
        .dataSize = readWriteSize(&readWrite),
        .pkg = &readWrite,
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

uint64_t queryStateData(char *outData, uint64_t outDataSize, const char *name,
                        uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, state_flag_t state_flag, header_flag_t header_flags)
{
    if (outData == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    state_pkg_t state = {
        .nameSize = (uint8_t)strlen(name),
        .state = State_STATE_OFFLINE,
        .flags = state_flag,
        .dataSize = 0,
        .name = (char *)name,
        .data = NULL
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_STATE,
        .flags = header_flags,
        .fragment = 1,
        .fragments = 1,
        .dataSize = stateSize(&state),
        .pkg = &state
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

iotv_obj_t *createIotFromHeaderIdentification(const header_t *header)
{
    if (header == NULL || header->pkg == NULL)
    {
        RETURN_WARNING;
        return NULL;
    }

    identification_t *ptrIdentification = ((identification_t *)header->pkg);

    uint8_t nameSize = ptrIdentification->nameSize;
    uint16_t descriptionSize = ptrIdentification->descriptionSize;

    uint8_t numberWriteChannel = ptrIdentification->numberWriteChannel;
    uint8_t numberReadChannel = ptrIdentification->numberReadChannel;

    iotv_obj_t *ptrIot = (iotv_obj_t *)malloc(sizeof(iotv_obj_t));
    if (ptrIot == NULL)
    {
        RETURN_WARNING;
        return NULL;
    }

    iotv_obj_t iot = {
        .id = ptrIdentification->id,
        .numberReadChannel = numberReadChannel,
        .numberWriteChannel = numberWriteChannel,
        .state = false,
        .nameSize = nameSize,
        .descriptionSize = descriptionSize,
        .readChannel = (numberReadChannel > 0) ? (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * numberReadChannel) : NULL,
        .readChannelType = (numberReadChannel > 0) ? (uint8_t *)malloc(numberReadChannel) : NULL,
        .writeChannelType = (numberWriteChannel > 0) ? (uint8_t *)malloc(numberWriteChannel) : NULL,
        .name = (nameSize > 0) ? (char *)malloc(nameSize) : NULL,
        .description = (descriptionSize > 0) ? (char *)malloc(descriptionSize) : NULL
    };

    if (nameSize > 0)
        memcpy(iot.name, ptrIdentification->name, nameSize);
    if (descriptionSize > 0)
        memcpy(iot.description, ptrIdentification->description, descriptionSize);

    bool mallocFlag = false;
    for (int i = 0; i < numberReadChannel; ++i)
    {
        iot.readChannel[i].dataSize = dataSizeonDataType(ptrIdentification->readChannelType[i]);
        iot.readChannel[i].data = (char *)malloc(iot.readChannel[i].dataSize);
        if (iot.readChannel[i].dataSize > 0 && iot.readChannel[i].data == NULL)
        {
            // не удалось выделить память!
            mallocFlag = true;
            break;
        }
        iot.readChannelType[i] = ptrIdentification->readChannelType[i];
    }

    for (int i = 0; i < numberWriteChannel; ++i)
        iot.writeChannelType[i] = ptrIdentification->writeChannelType[i];

    memcpy(ptrIot, &iot, sizeof(iotv_obj_t));

    // Проверка malloc!
    if (mallocFlag
        ||(nameSize > 0 && ptrIot->name == NULL)
        || (descriptionSize > 0 && ptrIot->description == NULL)
        || (numberReadChannel > 0 && (ptrIot->readChannel == NULL || ptrIot->readChannelType == NULL))
        || (numberWriteChannel > 0 && ptrIot->writeChannelType == NULL))
    {
        RETURN_WARNING;
        clear_iotv_obj(ptrIot);
        return NULL;
    }

    return ptrIot;
}


uint64_t responseTech(char *outData, uint64_t outDataSize, const char *rawData, uint64_t rawDataSize, const header_t *head,
                      uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, tech_flag_t flags, header_flag_t header_flags)
{
    if (outData == NULL || rawData == NULL || head == NULL || head->pkg == NULL || writeFunc == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    const tech_t *pkg = (const tech_t *)(head->pkg);
    uint64_t pkgsCount;
    uint64_t totalSendByte = 0;

    char *it = (char *)rawData;
    char *end = (char *)rawData + rawDataSize;

    pkgsCount = pkgCount(rawDataSize, outDataSize, HEADER_SIZE + TECH_SIZE);

    // Максимальное значение uint16_t = 0xFFFF. fragments в header не может привышать данное значение!
    if (pkgsCount > (uint64_t)0xFFFF)
    {
        RETURN_WARNING;
        return 0;
    }

    for (uint16_t i = 0; i < pkgsCount; ++i)
    {
        uint64_t rawDataPart = outDataSize - HEADER_SIZE - TECH_SIZE;
        uint64_t sizeLeft = end - it;
        if (sizeLeft < rawDataPart)
            rawDataPart = sizeLeft;

        tech_t tech;
        memcpy(&tech, pkg, sizeof(tech));
        tech.dataSize = rawDataPart;
        tech.data = (uint8_t *)it;
        tech.flags = flags;

        header_t header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_TECH,
            .flags = header_flags,
            .fragment = i + 1,
            .fragments = (uint16_t)(pkgsCount),
            .dataSize = techSize(&tech),
            .pkg = &tech
        };

        it += rawDataPart;

        uint64_t resultSize = headerToData(&header, outData, outDataSize);
        totalSendByte += writeFunc(outData, resultSize, obj);
    }

    return totalSendByte;
}

uint64_t responseLogData(const char *rawData, uint64_t rawDataSize, char *outData, uint64_t outDataSize, const struct Log_Data *pkg,
                         uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj)
{
    if (outData == NULL || outDataSize == 0 || pkg == NULL || writeFunc == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    // Пустые данные
    if (rawData == NULL || rawDataSize == 0)
    {
        log_data_t logData;
        memcpy(&logData, pkg, sizeof(logData));
        logData.dataSize = 0;
        logData.data = NULL;

        header_t header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_LOG_DATA,
            .flags = HEADER_FLAGS_NONE,
            .fragment = 1,
            .fragments = 1,
            .dataSize = logDataSize(&logData),
            .pkg = &logData
        };

        uint64_t resultSize = headerToData(&header, outData, outDataSize);
        return  writeFunc(outData, resultSize, obj);
    }

    uint64_t pkgsCount;
    uint64_t totalSendByte = 0;

    char *it = (char *)rawData;
    char *end = (char *)rawData + rawDataSize;

    pkgsCount = pkgCount(rawDataSize, outDataSize, HEADER_SIZE + LOG_DATA_SIZE + pkg->nameSize);

    // Максимальное значение uint16_t = 0xFFFF. fragments в header не может привышать данное значение!
    if (pkgsCount > (uint64_t)0xFFFF)
    {
        RETURN_WARNING;
        return 0;
    }

    for (uint16_t i = 0; i < pkgsCount; ++i)
    {
        uint64_t rawDataPart = outDataSize - HEADER_SIZE - LOG_DATA_SIZE - pkg->nameSize;
        uint64_t sizeLeft = end - it;
        if (sizeLeft < rawDataPart)
            rawDataPart = sizeLeft;

        log_data_t logData;
        memcpy(&logData, pkg, sizeof(logData));
        logData.dataSize = rawDataPart;
        logData.data = it;

        header_t header = {
            .version = 2,
            .type = HEADER_TYPE_RESPONSE,
            .assignment = HEADER_ASSIGNMENT_LOG_DATA,
            .flags = HEADER_FLAGS_NONE,
            .fragment = (uint16_t)(i + 1),
            .fragments = (uint16_t)(pkgsCount),
            .dataSize = logDataSize(&logData),
            .pkg = &logData
        };

        it += rawDataPart;

        uint64_t resultSize = headerToData(&header, outData, outDataSize);
        totalSendByte += writeFunc(outData, resultSize, obj);
    }

    return totalSendByte;
}

uint64_t queryTech(char *outData, uint64_t outDataSize, const char *inData, uint64_t inDataSize, tech_type_t type,
                   uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, tech_flag_t tech_flag, header_flag_t header_flags)
{
    if (outData == NULL || (inData == NULL && inDataSize > 0))
    {
        RETURN_WARNING;
        return 0;
    }

    tech_t tech = {
        .flags = tech_flag,
        .type = type,
        .dataSize = inDataSize,
        .data = (uint8_t *)inData
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_TECH,
        .flags = header_flags,
        .fragment = 1,
        .fragments = 1,
        .dataSize = techSize(&tech),
        .pkg = &tech
    };

    uint64_t resultSize = headerToData(&header, outData, outDataSize);
    return writeFunc(outData, resultSize, obj);
}

static uint64_t responceReadWritePkgCount(uint64_t dataOutSize, const iotv_obj_t *iot, const header_t *header)
{
    if (header == NULL || header->pkg == NULL || iot->readChannel == NULL || iot == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    read_write_t *ptrReadWrite = ((read_write_t *)header->pkg);
    if (iot->numberReadChannel <= ptrReadWrite->channelNumber)
    {
        RETURN_WARNING;
        return 0;
    }

    uint64_t sendDataSize = iot->readChannel[ptrReadWrite->channelNumber].dataSize;
    return pkgCount(sendDataSize, dataOutSize, HEADER_SIZE + READ_WRITE_SIZE + iot->nameSize);
}

uint64_t queryLogData(char *outData, uint64_t outDataSize, const char *name,
                      uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, log_data_flag_t flags)
{
    if (outData == NULL)
    {
        RETURN_WARNING;
        return 0;
    }

    log_data_t logData = {
        .startInterval = startInterval,
        .endInterval = endInterval,
        .interval = interval,
        .dataSize = 0,
        .nameSize = (uint8_t)strlen(name),
        .channelNumber = channelNumber,
        .flags = flags,
        .name = (char *)name,
        .data = NULL
    };

    header_t header = {
        .version = 2,
        .type = HEADER_TYPE_REQUEST,
        .assignment = HEADER_ASSIGNMENT_LOG_DATA,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = logDataSize(&logData),
        .pkg = &logData,
    };

    return headerToData(&header, outData, outDataSize);
}
