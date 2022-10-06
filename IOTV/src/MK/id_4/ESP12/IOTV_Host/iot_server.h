#pragma once

#include "IOTV_SH.h"

#define READ_CHANNEL_LENGTH 4
#define WRITE_CHANNEL_LENGTH 4

class IOTV_Server
{
public:
    IOTV_Server();

    const uint8_t _id;
    const char *_description;

    uint8_t _readChannel[READ_CHANNEL_LENGTH];

    Protocol_class::DATA_TYPE _readChannelType[READ_CHANNEL_LENGTH];
    Protocol_class::DATA_TYPE _writeChannelType[WRITE_CHANNEL_LENGTH];

    bool play() const;
    void setPlay(bool newPlay);

    bool led() const;
    void setLed(bool newLed);

    bool repeate() const;
    void setRepeate(bool newRepeate);

    uint8_t mode() const;
    void setMode(uint8_t newMode);

private:
    uint8_t &_play, &_led, &_repeate, &_mode;
};
