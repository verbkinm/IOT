#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <iostream>
#include <QTimer>

#include "raw.h"
#include "log.h"

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

    Raw::RAW _readChannel[READ_CHANNEL_LENGTH];
    Raw::RAW _writeChannel[WRITE_CHANNEL_LENGTH];

    Raw::DATA_TYPE _readChannelType[READ_CHANNEL_LENGTH];
    Raw::DATA_TYPE _writeChannelType[WRITE_CHANNEL_LENGTH];

    void update();
};

#endif // IOT_SERVER_H
