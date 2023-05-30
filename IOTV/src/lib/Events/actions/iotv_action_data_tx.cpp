#include "iotv_action_data_tx.h"

IOTV_Action_Data_TX::IOTV_Action_Data_TX(Base_Host *host,
                                         uint8_t channelNumber, const Raw &raw,
                                         QObject *parent) :
    IOTV_Action(ACTION_TYPE::DATA_TX, parent),
    _host(host),
    _channelNumber(channelNumber),
    _data(raw)
{

}

void IOTV_Action_Data_TX::exec()
{
    if (isValid())
        emit _host->signalQueryWrite(_channelNumber, _data.data());
}

bool IOTV_Action_Data_TX::isValid() const
{
    return !(type() == IOTV_Action::ACTION_TYPE::NONE || _host == nullptr);
}

const Base_Host *IOTV_Action_Data_TX::host() const
{
    return _host;
}

uint8_t IOTV_Action_Data_TX::channelNumber() const
{
    return _channelNumber;
}

const Raw &IOTV_Action_Data_TX::data() const
{
    return _data;
}
