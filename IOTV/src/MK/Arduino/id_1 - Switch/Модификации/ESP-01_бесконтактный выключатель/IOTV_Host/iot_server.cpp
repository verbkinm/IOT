#include "iot_server.h"

IOT_Server::IOT_Server() : _id(1), _description("ESP01-switch")
{
    _readChannelType[0] = Raw::DATA_TYPE::BOOL_8;
    _writeChannelType[0] = Raw::DATA_TYPE::BOOL_8;
}
