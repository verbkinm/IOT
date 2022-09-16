#include "iot_server.h"

IOTV_Server::IOTV_Server() : _id(1), _description("switch")
{
    _readChannel[0] = -1;
    _readChannel[1] = -255;
    _readChannel[2] = 255;

    _readChannelType[0] = Protocol_class::DATA_TYPE::INT_16;
    _readChannelType[1] = Protocol_class::DATA_TYPE::INT_16;
    _readChannelType[2] = Protocol_class::DATA_TYPE::INT_16;

    _writeChannelType[0] = Protocol_class::DATA_TYPE::INT_16;
    _writeChannelType[1] = Protocol_class::DATA_TYPE::INT_16;
    _writeChannelType[2] = Protocol_class::DATA_TYPE::INT_16;
}
