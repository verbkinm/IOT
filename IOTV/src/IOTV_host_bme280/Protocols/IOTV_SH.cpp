#include "IOTV_SH.h"
#include "iot_server.h"


uint16_t Protocol_class::response_WAY(const IOT_Server &iotHost, char *outData)
{
    uint16_t descriptionLength = iotHost._description.size();
    uint8_t channelRead = READ_CHANNEL_LENGTH;
    uint8_t channelWrite = WRITE_CHANNEL_LENGTH;
    uint16_t dataSize = 5 + descriptionLength + channelRead + channelWrite;

    outData[0] = RESPONSE_WAY_BYTE;
    outData[1] = iotHost._id;
    outData[2] = iotHost._description.size() << 8;
    outData[3] = iotHost._description.size();

    outData[4] = (channelRead << 4) | channelWrite;

    memcpy(&outData[5], iotHost._description.data(), descriptionLength);
    memcpy(&outData[5 + descriptionLength], iotHost._readChannelType, channelRead);

    return dataSize;
}

uint16_t Protocol_class::response_READ(const IOT_Server &iotHost, const char *inData, char *outData)
{
    uint8_t channelNumber = inData[0] >> 4;
    auto value = iotHost._readChannel[channelNumber];
    char *arr = reinterpret_cast<char*>(&value);
    uint16_t valueSize = sizeof(value);
    uint16_t dataSize = 3 + valueSize;

    outData[0] = (channelNumber << 4) | Protocol_class::RESPONSE_READ_BYTE;
    outData[1] = valueSize << 8;
    outData[2] = valueSize;

    memcpy(&outData[3], arr, valueSize);

    return dataSize;
}

uint16_t Protocol_class::response_Pong(char *outData)
{
    outData[0] = Protocol_class::RESPONSE_PONG_BYTE;
    return 1;
}
