#include "iot_server.h"

IOTV_Server::IOTV_Server() : _id(1), _description("Switch and vl6180")
{
  _readChannelType[0] = Protocol_class::DATA_TYPE::BOOL;
  _writeChannelType[0] = Protocol_class::DATA_TYPE::BOOL;

  _readChannel[0] = false;
}