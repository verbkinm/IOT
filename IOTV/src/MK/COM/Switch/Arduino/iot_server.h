#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <WString.h>
#include "raw.h"

#define READ_CHANNEL_LENGTH 1
#define WRITE_CHANNEL_LENGTH 1

class IOT_Server
{
public:
    IOT_Server();

    const uint8_t _id;
    const String _description;

    Raw::RAW _readChannel[READ_CHANNEL_LENGTH];
    Raw::RAW _writeChannel[WRITE_CHANNEL_LENGTH];

    Raw::DATA_TYPE _readChannelType[READ_CHANNEL_LENGTH];
    Raw::DATA_TYPE _writeChannelType[WRITE_CHANNEL_LENGTH];

    void update();
};

#endif // IOT_SERVER_H
