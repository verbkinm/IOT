#pragma once

#include "IOTV_SH.h"

#define READ_CHANNEL_LENGTH 3
#define WRITE_CHANNEL_LENGTH 3

struct IOTV_Server
{
    const uint8_t id;
    const char *name;
    const char *description;

    bool readChannel[READ_CHANNEL_LENGTH];
    DATA_TYPE readChannelType[READ_CHANNEL_LENGTH];

    DATA_TYPE writeChannelType[WRITE_CHANNEL_LENGTH];
};
