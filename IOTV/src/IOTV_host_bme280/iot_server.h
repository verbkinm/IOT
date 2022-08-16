#pragma once

#include <iostream>

#include "log.h"
#include "Protocols/IOTV_SH.h"

#define READ_CHANNEL_LENGTH 3
#define WRITE_CHANNEL_LENGTH 0

class IOT_Server : public QObject
{
    Q_OBJECT

public:
    IOT_Server();

    void newValue();

    const uint8_t _id;
    const std::string _description;

    float _readChannel[READ_CHANNEL_LENGTH];
    uint8_t _readChannelType[READ_CHANNEL_LENGTH];

    float _writeChannel[WRITE_CHANNEL_LENGTH];
    uint8_t _writeChannelType[WRITE_CHANNEL_LENGTH];

    void update();
};
