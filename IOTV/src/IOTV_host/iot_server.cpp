#include "iot_server.h"

IOT_Server::IOT_Server() : _id(1), _description("switch")
{
    _readChannel[0].ui8 = 0;
    _readChannelType[0] = Raw::DATA_TYPE::UNSIGNED_INTEGER_8;
}

void IOT_Server::update()
{
    _readChannel[0].ui8 = _writeChannel[0].ui8;
}
