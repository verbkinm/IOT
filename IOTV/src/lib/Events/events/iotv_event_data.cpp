#include "iotv_event_data.h"

IOTV_Event_Data::IOTV_Event_Data(const DATA_DIRECTION &direction, std::function<bool(Raw, Raw)> compare,
                                 const Base_Host *host,
                                 uint8_t channelNumber, const Raw &raw,
                                 QObject *parent) :
    IOTV_Event(EVENT_TYPE::DATA, host, parent),
    _type(direction),
    _channelNumber(channelNumber),
    _data(raw),
    _compare(compare)
{
    if (host == nullptr)
        return;

    if (direction == DATA_DIRECTION::RX)
        connect(host, &Base_Host::signalDataRX, this, &IOTV_Event_Data::slotCheckData, Qt::UniqueConnection);
    else if (direction == DATA_DIRECTION::TX)
        connect(host, &Base_Host::signalDataTX, this, &IOTV_Event_Data::slotCheckData, Qt::UniqueConnection);
    else if (direction == DATA_DIRECTION::ANY)
    {
        connect(host, &Base_Host::signalDataRX, this, &IOTV_Event_Data::slotCheckData, Qt::UniqueConnection);
        connect(host, &Base_Host::signalDataTX, this, &IOTV_Event_Data::slotCheckData, Qt::UniqueConnection);
    }
    else if (direction == DATA_DIRECTION::CHANGE)
        connect(host, &Base_Host::signalDataChanged, this, &IOTV_Event_Data::slotCheckData, Qt::UniqueConnection);
}

IOTV_Event_Data::DATA_DIRECTION IOTV_Event_Data::type() const
{
    return _type;
}

void IOTV_Event_Data::slotCheckData(uint8_t channleNumber, QByteArray rhs)
{
    Raw rawHost(_host->getReadChannelType(channleNumber), rhs);
    if (_channelNumber == channleNumber && _compare(_data, rawHost))
        emit signalEvent();
}
