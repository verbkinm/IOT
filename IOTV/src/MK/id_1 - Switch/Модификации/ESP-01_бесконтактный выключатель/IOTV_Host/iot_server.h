#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include "raw.h"

#define READ_CHANNEL_LENGTH 1
#define WRITE_CHANNEL_LENGTH 1

class IOT_Server
{
public:
    IOT_Server();

    const iot_uint8_t _id;
    const char* _description;

    Raw::RAW _readChannel[READ_CHANNEL_LENGTH];
    Raw::RAW _writeChannel[WRITE_CHANNEL_LENGTH];

    Raw::DATA_TYPE _readChannelType[READ_CHANNEL_LENGTH];
    Raw::DATA_TYPE _writeChannelType[WRITE_CHANNEL_LENGTH];
};

#endif // IOT_SERVER_H
