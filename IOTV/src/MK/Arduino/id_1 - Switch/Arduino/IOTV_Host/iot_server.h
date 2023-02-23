#pragma once

#include "IOTV_SH.h"

#define READ_CHANNEL_LENGTH 1
#define WRITE_CHANNEL_LENGTH 1

class IOTV_Server
{
public:
    IOTV_Server();

    const uint8_t _id;
    const char *_description;

    uint8_t _readChannel[READ_CHANNEL_LENGTH];

    Protocol_class::DATA_TYPE _readChannelType[READ_CHANNEL_LENGTH];
    Protocol_class::DATA_TYPE _writeChannelType[WRITE_CHANNEL_LENGTH];
};
