#include "iot_server.h"

IOT_Server::IOT_Server() : _id(2), _description("DHT22-01")
{
    _readChannelType[0] = Raw::DATA_TYPE::FLOAT_32;
    _readChannelType[1] = Raw::DATA_TYPE::FLOAT_32;
    //_readChannelType[2] = Raw::DATA_TYPE::FLOAT_32;
}
