#include "iot_server.h"

IOTV_Server::IOTV_Server()
  : _id(1), _description("ESP01 + Arduino Pro mini + Rele + vl6180")
{
  _readChannel[0] = false;

  _readChannelType[0] = Protocol_class::DATA_TYPE::BOOL;
  _writeChannelType[0] = Protocol_class::DATA_TYPE::BOOL;

  // memset(_readChannel, 0, READ_CHANNEL_LENGTH);

  // memset(_readChannelType, Protocol_class::DATA_TYPE::BOOL, READ_CHANNEL_LENGTH);
  // memset(_writeChannelType, Protocol_class::DATA_TYPE::BOOL, WRITE_CHANNEL_LENGTH);
}