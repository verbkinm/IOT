#include "iot_server.h"

IOT_Server::IOT_Server() : _id(0), _description("factory description"), _userDescription("user description")
{
    _readChannel[0].ui64 = 0;
    _readChannel[1].ui64 = 0;

    _readChannel[0].ui8 = 20;
    _readChannel[1].ui8 = 0xFF;

    _readChannelType[0] = Raw::DATA_TYPE::INTEGER_8;
    _readChannelType[1] = Raw::DATA_TYPE::UNSIGNED_INTEGER_8;

    _writeChannelType[0] = Raw::DATA_TYPE::INTEGER_16;

    Log::write("Start service");
}
