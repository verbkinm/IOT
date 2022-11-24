#include "IOTV_SH.h"
#include "iot_server.h"

uint16_t Protocol_class::response_WAY(const IOTV_Server &iotHost, char *outData)
{
    uint16_t descriptionLength = strlen(iotHost._description);
    uint8_t channelRead = READ_CHANNEL_LENGTH;
    uint8_t channelWrite = WRITE_CHANNEL_LENGTH;
    uint16_t dataSize = 5 + descriptionLength + channelRead + channelWrite;

    outData[0] = RESPONSE_WAY_BYTE;
    outData[1] = iotHost._id;
    outData[2] = strlen(iotHost._description) << 8;
    outData[3] = strlen(iotHost._description);

    outData[4] = (channelRead << 4) | channelWrite;

    memcpy(&outData[5], iotHost._description, descriptionLength);

    memcpy(&outData[5 + descriptionLength], iotHost._readChannelType, channelRead);
    memcpy(&outData[5 + descriptionLength + channelRead], iotHost._writeChannelType, channelWrite);

    return dataSize;
}

uint16_t Protocol_class::response_READ(const IOTV_Server &iotHost, const char *inData, const char *ptrInData, char *outData)
{
    uint16_t realDataSize = ptrInData - inData;

    if (realDataSize < 1)
        return 0;

    uint8_t channelNumber = inData[0] >> 4;

    uint16_t dataSize = 3,
            valueSize = 0;
    char *arr = 0;

    if ( channelNumber <= (READ_CHANNEL_LENGTH - 1) )
    {
        auto value = iotHost._readChannel[channelNumber];
        arr = reinterpret_cast<char*>(&value);
        valueSize = sizeof(value);
        dataSize += valueSize;
    }

    outData[0] = (channelNumber << 4) | Protocol_class::RESPONSE_READ_BYTE;
    outData[1] = valueSize << 8;
    outData[2] = valueSize;

    memcpy(&outData[3], arr, valueSize);

    return dataSize;
}

int Protocol_class::response_WRITE(IOTV_Server &iotHost, const char *inData, const char *ptrInData, char *outData)
{
    uint16_t realDataSize = ptrInData - inData;

    if (realDataSize < 3)
        return -1; //не запрос пришел полный

    uint8_t channelNumber = inData[0] >> 4;

    uint16_t dataWriteSize = (uint16_t(inData[1]) << 8) | inData[2];

    if (realDataSize < (3 + dataWriteSize))
        return -1; //не запрос пришел полный

    char writeData[dataWriteSize];
    memcpy(writeData, &inData[3], dataWriteSize);

    if ((dataWriteSize != 0) && (channelNumber <= (WRITE_CHANNEL_LENGTH - 1)))
        memcpy(&iotHost._readChannel[channelNumber], writeData, sizeof(iotHost._readChannel[channelNumber]));

    outData[0] = (channelNumber << 4) | Protocol_class::RESPONSE_WRITE_BYTE;

    return dataWriteSize;
}

uint16_t Protocol_class::response_Pong(char *outData)
{
    outData[0] = Protocol_class::RESPONSE_PONG_BYTE;
    return 1;
}
