#include "iotv_action_data_tx_ref.h"

IOTV_Action_Data_TX_Ref::IOTV_Action_Data_TX_Ref(Base_Host *dstHost, uint8_t dstChannelNumber,
                                                 Base_Host *srcHost, uint8_t srcChannelNumber,
                                                 QObject *parent) :
    IOTV_Action(ACTION_TYPE::DATA_TX, parent),
    _dstHost(dstHost),
    _srcHost(srcHost),
    _dstChannelNumber(dstChannelNumber),
    _srcChannelNumber(srcChannelNumber)
{

}

void IOTV_Action_Data_TX_Ref::exec()
{
    if (!isValid())
        return;

    Raw raw = _srcHost->getReadChannelDataRaw(_srcChannelNumber);
    if (raw.isValid())
        emit _dstHost->signalQueryWrite(_dstChannelNumber, raw.data());
}

bool IOTV_Action_Data_TX_Ref::isValid() const
{
    return !(type() == IOTV_Action::ACTION_TYPE::NONE || _dstHost == nullptr || _srcHost == nullptr);
}

const Base_Host *IOTV_Action_Data_TX_Ref::dstHost() const
{
    return _dstHost;
}

const Base_Host *IOTV_Action_Data_TX_Ref::srcHost() const
{
    return _srcHost;
}

uint8_t IOTV_Action_Data_TX_Ref::dstChannelNumber() const
{
    return _dstChannelNumber;
}

uint8_t IOTV_Action_Data_TX_Ref::srcChannelNumber() const
{
    return _srcChannelNumber;
}
