#include "iotv_action_data_tx.h"

IOTV_Action_Data_TX::IOTV_Action_Data_TX(Base_Host *host,
                                         uint8_t channelNumber, const QString &data,
                                         QObject *parent) :
    IOTV_Action(ACTION_TYPE::DATA_TX, parent),
    _host(host),
    _channelNumber(channelNumber),
    _data(data)
{
    if (host != nullptr)
        setHostName(host->getName());
}

void IOTV_Action_Data_TX::exec()
{
    if (!isEnable() || !isValid())
        return;

    Raw raw(_host->getReadChannelType(_channelNumber), _data);
    if (raw.isValid())
    {
        emit _host->signalQueryWrite(_channelNumber, raw.data());
        emit signalAction(group(), name(), getType());
    }
}

bool IOTV_Action_Data_TX::isValid() const
{
    return !(_host == nullptr || _host->getId() == 0 || type() != IOTV_Action::ACTION_TYPE::DATA_TX);
}

QString IOTV_Action_Data_TX::hostName() const
{
    return _hostName;
}

void IOTV_Action_Data_TX::setHostName(const QString &newHostName)
{
    if (_hostName == newHostName)
        return;

    _hostName = newHostName;
    emit signalHostNameChanged(newHostName);
}

const Base_Host *IOTV_Action_Data_TX::host() const
{
    return _host;
}

uint8_t IOTV_Action_Data_TX::channelNumber() const
{
    return _channelNumber;
}

QString IOTV_Action_Data_TX::data() const
{
    return _data;
}

void IOTV_Action_Data_TX::setDataStr(const QString &newDataStr)
{
    if (_data == newDataStr)
        return;

    _data = newDataStr;
    emit signalDataChanged(newDataStr);
}

void IOTV_Action_Data_TX::setChannelNumber(uint8_t newChNum)
{
    if (_channelNumber == newChNum)
        return;

    _channelNumber = newChNum;
    emit signalChannelNumberChanged(newChNum);
}

void IOTV_Action_Data_TX::forceExec()
{
    if (!isValid())
        return;

    Raw raw(_host->getReadChannelType(_channelNumber), _data);
    if (raw.isValid())
    {
        emit _host->signalQueryWrite(_channelNumber, raw.data());
        emit signalAction(group(), name(), getType());
    }
}
