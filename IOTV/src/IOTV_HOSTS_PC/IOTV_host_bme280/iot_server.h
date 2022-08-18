#pragma once

#include <iostream>

#include "Protocols/IOTV_SH.h"

#define READ_CHANNEL_LENGTH 3
#define WRITE_CHANNEL_LENGTH 0

class IOT_Server
{
public:
    IOT_Server();

    //для ПК
    void newValue();

    const uint8_t _id;
    const char* _description;

    float _readChannel[READ_CHANNEL_LENGTH];
    uint8_t _readChannelType[READ_CHANNEL_LENGTH];

//    float _writeChannel[WRITE_CHANNEL_LENGTH];
//    uint8_t _writeChannelType[WRITE_CHANNEL_LENGTH];

    void update();
};
