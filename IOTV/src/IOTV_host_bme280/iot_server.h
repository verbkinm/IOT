#pragma once

#include <iostream>
#include <QTimer>

#include "log.h"
#include "Protocols/IOTV_SH.h"

#define READ_CHANNEL_LENGTH 3
#define WRITE_CHANNEL_LENGTH 0

class IOT_Server : public QObject
{
    Q_OBJECT

    QTimer _timer;

    void newValue();

public:
    IOT_Server();

    const uint8_t _id;
    const std::string _description;

    float _readChannel[READ_CHANNEL_LENGTH];
    uint8_t _readChannelType[READ_CHANNEL_LENGTH];

    float _writeChannel[WRITE_CHANNEL_LENGTH];
    uint8_t _writeChannelType[WRITE_CHANNEL_LENGTH];

    void update();
};
