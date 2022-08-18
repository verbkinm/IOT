#include "iot_server.h"

IOT_Server::IOT_Server() : _id(1), _description("switch")
{
    _readChannel[0] = false;

    _readChannelType[0] = Protocol_class::DATA_TYPE::BOOL;
    _writeChannelType[0] = Protocol_class::DATA_TYPE::BOOL;
}
