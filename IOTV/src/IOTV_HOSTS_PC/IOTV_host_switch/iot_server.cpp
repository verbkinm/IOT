#include "iot_server.h"

IOTV_Server::IOTV_Server() : _id(1), _description("switch")
{
    _readChannel[0] = 0;
    _readChannel[1] = 0;
    _readChannel[2] = 0;

    _readChannelType[0] = Protocol_class::DATA_TYPE::BOOL;
    _readChannelType[1] = Protocol_class::DATA_TYPE::BOOL;
    _readChannelType[2] = Protocol_class::DATA_TYPE::BOOL;

    _writeChannelType[0] = Protocol_class::DATA_TYPE::BOOL;
    _writeChannelType[1] = Protocol_class::DATA_TYPE::BOOL;
    _writeChannelType[2] = Protocol_class::DATA_TYPE::BOOL;
}
