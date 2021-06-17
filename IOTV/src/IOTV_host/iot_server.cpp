#include "iot_server.h"

IOT_Server::IOT_Server() : _id(2), _description("factory description")
{
    _readChannel[0].ui64 = 0;

//    _readChannel[0].ui8 = 0;

    _readChannelType[0] = Raw::DATA_TYPE::FLOAT_32;
    _readChannelType[1] = Raw::DATA_TYPE::FLOAT_32;
    _readChannelType[2] = Raw::DATA_TYPE::FLOAT_32;

    _readChannel[0].f = 36.6;
    _readChannel[1].f = 20.67;
    _readChannel[2].f = 525.327;

//    _writeChannelType[0] = Raw::DATA_TYPE::UNSIGNED_INTEGER_8;
}

void IOT_Server::update()
{
//    _readChannel[0].ui8 = _writeChannel[0].ui8;
}
