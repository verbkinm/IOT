#pragma once

#include "IOTV_SH.h"

#define READ_CHANNEL_LENGTH 3
/*
0 - isPlay - текущее состояние музыки
1 - LED state - 
2 - Repeate state
*/
#define WRITE_CHANNEL_LENGTH 4
/*
0 - playStop - 1 == переключить состояния
1 - LED state
2 - Repeate state
3 - Mode type 0, 1, 2, 3
*/

class IOTV_Server
{
public:
    IOTV_Server();

    const uint8_t _id;
    const char *_description;

    uint8_t _readChannel[READ_CHANNEL_LENGTH];

    Protocol_class::DATA_TYPE _readChannelType[READ_CHANNEL_LENGTH];
    Protocol_class::DATA_TYPE _writeChannelType[WRITE_CHANNEL_LENGTH];

    bool playStop() const;
    void setPlayStop(bool newPlay);

    bool led() const;
    void setLed(bool newLed);

    bool repeate() const;
    void setRepeate(bool newRepeate);

    uint8_t mode() const;
    void setMode(uint8_t newMode);

    bool isPlaing() const;
    void setPlaing(bool state);

private:
    uint8_t &_isPlaing, &_led, &_repeate, _mode, _playStop;
};
