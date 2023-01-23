#pragma once

#include "Protocols/IOTV_SH.h"

#define READ_CHANNEL_LENGTH 3
#define WRITE_CHANNEL_LENGTH 3

enum DATA_TYPE
{
    DATA_TYPE_INT_8,
    DATA_TYPE_INT_16,
    DATA_TYPE_INT_32,
    DATA_TYPE_INT_64,

    DATA_TYPE_FLOAT_32,
    DATA_TYPE_DOUBLE_64, // на МК double 32-битный может быть

    DATA_TYPE_BOOL,
    DATA_TYPE_STRING,

    DATA_TYPE_RAW,
    DATA_TYPE_NONE
};

struct IOTV_Server
{
    const uint8_t id;
    const char *name;
    const char *description;

    bool readChannel[READ_CHANNEL_LENGTH];
    enum DATA_TYPE readChannelType[READ_CHANNEL_LENGTH];

    enum DATA_TYPE writeChannelType[WRITE_CHANNEL_LENGTH];
};
