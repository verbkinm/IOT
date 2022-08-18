#include "iot_server.h"

void IOT_Server::newValue()
{
    _readChannel[0] = (rand() % 100) + (float)(rand() % 100) / 100;
    _readChannel[1] = (rand() % 100) + (float)(rand() % 100) / 100;
    _readChannel[2] = (rand() % 1000) + (float)(rand() % 100) / 100;
}

IOT_Server::IOT_Server() : _id(2), _description("bme280")
{
    srand(time(0));
    _readChannel[0] = 0.1;
    _readChannel[1] = 128.6;
    _readChannel[2] = -5;

    _readChannelType[0] = Protocol_class::DATA_TYPE::FLOAT_32;
    _readChannelType[1] = Protocol_class::DATA_TYPE::FLOAT_32;
    _readChannelType[2] = Protocol_class::DATA_TYPE::FLOAT_32;
}

void IOT_Server::update()
{
//    _readChannel[0].ui8 = _writeChannel[0].ui8;
}
