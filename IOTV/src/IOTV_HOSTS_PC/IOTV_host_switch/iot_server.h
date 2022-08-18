#pragma once

#include "Protocols/IOTV_SH.h"

#define READ_CHANNEL_LENGTH 1
#define WRITE_CHANNEL_LENGTH 1

class IOT_Server
{
public:
    IOT_Server();

    const uint8_t _id;
    const char *_description;

    bool _readChannel[READ_CHANNEL_LENGTH];
    uint8_t _readChannelType[READ_CHANNEL_LENGTH];

    uint8_t _writeChannelType[WRITE_CHANNEL_LENGTH];
};
