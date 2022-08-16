#include "IOTV_SH.h"
#include "iot_server.h"


uint16_t Protocol_class::response_WAY(const IOT_Server &iotHost, char *&data)
{
    uint16_t descriptionLength = iotHost._description.size();
    uint8_t channelRead = READ_CHANNEL_LENGTH;
    uint8_t channelWrite = WRITE_CHANNEL_LENGTH;
    uint16_t dataSize = 5 + descriptionLength + channelRead + channelWrite;

    if (data != nullptr)
        delete[] data;

    data = new char[dataSize];

    data[0] = RESPONSE_WAY_BYTE;
    data[1] = iotHost._id;
    data[2] = iotHost._description.size() << 8;
    data[3] = iotHost._description.size();

    data[4] = (channelRead << 4) | channelWrite;

    memcpy(&data[5], iotHost._description.data(), descriptionLength);
    memcpy(&data[5 + descriptionLength], iotHost._readChannelType, channelRead);

    return dataSize;
}

uint16_t Protocol_class::response_READ(const IOT_Server &iotHost, uint8_t channelNumber, char *&data)
{
    auto value = iotHost._readChannel[channelNumber];
    char *arr = reinterpret_cast<char*>(&value);
    uint16_t valueSize = sizeof(value);
    uint16_t dataSize = 3 + valueSize;

    if (data != nullptr)
        delete[] data;

    data = new char[dataSize];

    data[0] = (channelNumber << 4) | Protocol_class::RESPONSE_READ_BYTE;
    data[1] = valueSize << 8;
    data[2] = valueSize;

    memcpy(&data[3], arr, valueSize);

    return dataSize;
}

uint16_t Protocol_class::response_Pong(char *&data)
{
    if (data != nullptr)
        delete[] data;

    data = new char[1];

    data[0] = Protocol_class::RESPONSE_PONG_BYTE;

    return 1;
}
