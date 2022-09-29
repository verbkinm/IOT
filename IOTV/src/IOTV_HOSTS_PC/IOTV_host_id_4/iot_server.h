#pragma once

#include "Protocols/IOTV_SH.h"

#define READ_CHANNEL_LENGTH 3
#define WRITE_CHANNEL_LENGTH 6

class IOTV_Server
{
public:
    IOTV_Server();

    const uint8_t _id;
    const char *_description;

    bool _readChannel[READ_CHANNEL_LENGTH];
    Protocol_class::DATA_TYPE _readChannelType[READ_CHANNEL_LENGTH];

    bool _writeChannelType[WRITE_CHANNEL_LENGTH];
};
