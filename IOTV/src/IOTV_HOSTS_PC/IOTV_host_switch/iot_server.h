#pragma once

#include "Protocols/IOTV_SH.h"

#define READ_CHANNEL_LENGTH 7
#define WRITE_CHANNEL_LENGTH 3
class IOTV_Server
{
public:
    IOTV_Server();

    const uint8_t _id;
    const char *_description;

    double _readChannel[READ_CHANNEL_LENGTH];
    uint8_t _readChannelType[READ_CHANNEL_LENGTH];

    uint8_t _writeChannelType[WRITE_CHANNEL_LENGTH];
};
