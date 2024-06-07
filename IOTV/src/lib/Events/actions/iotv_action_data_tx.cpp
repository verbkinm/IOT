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
        emit _host->signalQueryWrite(_channelNumber, raw.data());
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
    _hostName = newHostName;
}

const Base_Host *IOTV_Action_Data_TX::host() const
{
    return _host;
}

uint8_t IOTV_Action_Data_TX::channelNumber() const
{
    return _channelNumber;
}

const QString &IOTV_Action_Data_TX::data() const
{
    return _data;
}
