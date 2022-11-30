#include "iot_server.h"

IOTV_Server::IOTV_Server()
  : _id(4), _description("Облачко для Евы-королевы!"),
    _isPlaing(_readChannel[0]), _led(_readChannel[1]), _repeate(_readChannel[2]), _mode(0), _playStop(0)
{
  std::memset(_readChannel, 0, READ_CHANNEL_LENGTH - 1);
  setLed(true);

  std::memset(_readChannelType, Protocol_class::DATA_TYPE::BOOL, READ_CHANNEL_LENGTH);
  std::memset(_writeChannelType, Protocol_class::DATA_TYPE::BOOL, WRITE_CHANNEL_LENGTH - 1);
  _writeChannelType[3] = Protocol_class::DATA_TYPE::INT_8;
}

bool IOTV_Server::playStop() const {
  return _playStop;
}

void IOTV_Server::setPlayStop(bool newPlay) {
  _playStop = newPlay;
}

bool IOTV_Server::led() const {
  return _led;
}

void IOTV_Server::setLed(bool newLed) {
  _led = newLed;
}

bool IOTV_Server::repeate() const {
  return _repeate;
}

void IOTV_Server::setRepeate(bool newRepeate) {
  _repeate = newRepeate;
}

uint8_t IOTV_Server::mode() const {
  return _mode;
}

void IOTV_Server::setMode(uint8_t newMode) {
  _mode = newMode;
}

bool IOTV_Server::isPlaing() const {
  return _isPlaing;
}
void IOTV_Server::setPlaing(bool state) {
  _isPlaing = state;
}