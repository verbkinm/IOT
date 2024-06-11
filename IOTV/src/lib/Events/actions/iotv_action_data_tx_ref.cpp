#include "iotv_action_data_tx_ref.h"

IOTV_Action_Data_TX_Ref::IOTV_Action_Data_TX_Ref(Base_Host *dstHost, uint8_t dstChannelNumber,
                                                 Base_Host *srcHost, uint8_t srcChannelNumber,
                                                 QObject *parent) :
    IOTV_Action(ACTION_TYPE::DATA_TX_REF, parent),
    _dstHost(dstHost),
    _srcHost(srcHost),
    _dstChannelNumber(dstChannelNumber),
    _srcChannelNumber(srcChannelNumber)
{
    if (dstHost != nullptr)
        setDstHostName(dstHost->getName());

    if (srcHost != nullptr)
        setSrcHostName(srcHost->getName());
}

void IOTV_Action_Data_TX_Ref::exec()
{
    if (!isEnable() || !isValid())
        return;

    Raw raw = _srcHost->getReadChannelDataRaw(_srcChannelNumber);
    if (raw.isValid())
        emit _dstHost->signalQueryWrite(_dstChannelNumber, raw.data());
}

bool IOTV_Action_Data_TX_Ref::isValid() const
{
    return !(_dstHost == nullptr || _dstHost->getId() == 0 || _srcHost == nullptr || _srcHost->getId() == 0 || type() != IOTV_Action::ACTION_TYPE::DATA_TX_REF);
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

QString IOTV_Action_Data_TX_Ref::srcHostName() const
{
    return _srcHostName;
}

void IOTV_Action_Data_TX_Ref::setSrcHostName(const QString &newSrcHostName)
{
    if (_srcHostName == newSrcHostName)
        return;

    _srcHostName = newSrcHostName;
    emit signalSrcHostChanged(newSrcHostName);
}

QString IOTV_Action_Data_TX_Ref::dstHostName() const
{
    return _dstHostName;
}

void IOTV_Action_Data_TX_Ref::setSrcChNum(uint8_t ch)
{
    if (_srcChannelNumber == ch)
        return;

    _srcChannelNumber = ch;
    emit signalSrcChNumChanged(ch);
}

void IOTV_Action_Data_TX_Ref::setDstChNum(uint8_t ch)
{
    if (_dstChannelNumber == ch)
        return;

    _dstChannelNumber = ch;
    emit signalDstChNumChanged(ch);
}

void IOTV_Action_Data_TX_Ref::setDstHostName(const QString &newDstHostName)
{
    if (_dstHostName == newDstHostName)
        return;

    _dstHostName = newDstHostName;
    emit signalDstHostChanged(newDstHostName);
}
