#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <iostream>

#include "../lib/raw/raw.h"
#include "../lib/Log/log.h"

#define READ_CHANNEL_LENGTH 2
#define WRITE_CHANNEL_LENGTH 1

class IOT_Server
{
public:
    IOT_Server();

    std::string _name;
    const uint8_t _id;
    const std::string _description;
    std::string _userDescription;

    Raw::RAW _readChannel[READ_CHANNEL_LENGTH];
    Raw::RAW _writeChannel[WRITE_CHANNEL_LENGTH];

    Raw::DATA_TYPE _readChannelType[READ_CHANNEL_LENGTH];
    Raw::DATA_TYPE _writeChannelType[READ_CHANNEL_LENGTH];
};

#endif // IOT_SERVER_H
