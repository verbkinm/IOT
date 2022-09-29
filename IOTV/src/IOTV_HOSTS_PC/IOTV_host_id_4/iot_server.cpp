#include "iot_server.h"

IOTV_Server::IOTV_Server() : _id(4), _description("Облачко для любимой доченьки")
{
    std::memset(_readChannel, false, READ_CHANNEL_LENGTH);

    for (uint8_t i = 0; i < READ_CHANNEL_LENGTH; i++)
        _readChannelType[i] = Protocol_class::DATA_TYPE::BOOL;

    for (uint8_t i = 0; i < WRITE_CHANNEL_LENGTH; i++)
        _writeChannelType[i] = Protocol_class::DATA_TYPE::BOOL;
}
