#include "iot_server.h"

IOTV_Server::IOTV_Server() : _id(4), _description("Облачко для любимой доченьки"),
    _play(_readChannel[0]), _led(_readChannel[1]), _repeate(_readChannel[2]), _mode(_readChannel[3])
{
    std::memset(_readChannel, 0, READ_CHANNEL_LENGTH);

    for (uint8_t i = 0; i < READ_CHANNEL_LENGTH - 1; i++)
        _readChannelType[i] = Protocol_class::DATA_TYPE::BOOL;

    for (uint8_t i = 0; i < WRITE_CHANNEL_LENGTH - 1; i++)
        _writeChannelType[i] = Protocol_class::DATA_TYPE::BOOL;

    _readChannelType[3] = Protocol_class::DATA_TYPE::INT_8;
    _writeChannelType[3] = Protocol_class::DATA_TYPE::INT_8;
}

bool IOTV_Server::play() const
{
    return _play;
}

void IOTV_Server::setPlay(bool newPlay)
{
    _play = newPlay;
}

bool IOTV_Server::led() const
{
    return _led;
}

void IOTV_Server::setLed(bool newLed)
{
    _led = newLed;
}

bool IOTV_Server::repeate() const
{
    return _repeate;
}

void IOTV_Server::setRepeate(bool newRepeate)
{
    _repeate = newRepeate;
}

uint8_t IOTV_Server::mode() const
{
    return _mode;
}

void IOTV_Server::setMode(uint8_t newMode)
{
    _mode = newMode;
}
