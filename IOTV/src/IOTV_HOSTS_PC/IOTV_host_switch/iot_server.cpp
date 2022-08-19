#include "iot_server.h"

IOTV_Server::IOTV_Server() : _id(1), _description("switch")
{
    _readChannel[0] = false;

    _readChannelType[0] = Protocol_class::DATA_TYPE::BOOL;
    _writeChannelType[0] = Protocol_class::DATA_TYPE::BOOL;
}
