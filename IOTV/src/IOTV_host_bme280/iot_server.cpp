#include "iot_server.h"
#include <time.h>

void IOT_Server::newValue()
{
    _readChannel[0].f = (rand() % 100) + (float)(rand() % 100) / 100;
    _readChannel[1].f = (rand() % 100) + (float)(rand() % 100) / 100;
    _readChannel[2].f = (rand() % 1000) + (float)(rand() % 100) / 100;
}

IOT_Server::IOT_Server() : _id(2), _description("bme280")
{
    srand(time(0));
    _readChannel[0].f = 0;
    _readChannel[1].f = 0;
    _readChannel[2].f = 0;

    _readChannelType[0] = Raw::DATA_TYPE::FLOAT_32;
    _readChannelType[1] = Raw::DATA_TYPE::FLOAT_32;
    _readChannelType[2] = Raw::DATA_TYPE::FLOAT_32;

    connect(&_timer, &QTimer::timeout, this, &IOT_Server::newValue);
    _timer.start(1000);
}

void IOT_Server::update()
{
//    _readChannel[0].ui8 = _writeChannel[0].ui8;
}
