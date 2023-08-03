#include "iotv_event_data.h"

IOTV_Event_Data::IOTV_Event_Data(const DATA_DIRECTION &direction, QString compare,
                                 const Base_Host *host,
                                 uint8_t channelNumber, const QString &raw,
                                 QObject *parent) :
    IOTV_Event(EVENT_TYPE::DATA, host, parent),
    _type(direction),
    _channelNumber(channelNumber),
    _data(raw),
    _compareStr(compare),
    _compare(createCompare(compare))
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

IOTV_Event_Data::DATA_DIRECTION IOTV_Event_Data::direction() const
{
    return _type;
}

std::function<bool(Raw, Raw)> IOTV_Event_Data::createCompare(const QString &compare)
{
    if (_type == DATA_DIRECTION::CHANGE && _compareStr.isEmpty())
        return [](Raw, Raw)->bool
        {
            return true;
        };

    if (compare == Json_Event_Action::COMPARE_EQ)
        return std::equal_to<Raw>();
    else if (compare == Json_Event_Action::COMPARE_NE)
        return std::not_equal_to<Raw>();
    else if (compare == Json_Event_Action::COMPARE_GR)
        return std::greater<Raw>();
    else if (compare == Json_Event_Action::COMPARE_LS)
        return std::less<Raw>();
    else if (compare == Json_Event_Action::COMPARE_GE)
        return std::greater_equal<Raw>();
    else if (compare == Json_Event_Action::COMPARE_LE)
        return std::less_equal<Raw>();
    else if (compare == Json_Event_Action::COMPARE_ALWAYS_TRUE)
    {
        return [](Raw, Raw)->bool
        {
            return true;
        };
    }

    return [](Raw, Raw)->bool
    {
        return false;
    };
}

const QString &IOTV_Event_Data::compareStr() const
{
    return _compareStr;
}

uint8_t IOTV_Event_Data::channelNumber() const
{
    return _channelNumber;
}

const QString IOTV_Event_Data::data() const
{
    return _data;
}

void IOTV_Event_Data::slotCheckData(uint8_t channleNumber, QByteArray rhs)
{
    if (_host->getId() == 0)
        return;


    Raw rawHost(_host->getReadChannelType(channleNumber), rhs);
    QString str = rawHost.strData().first;
    bool res = _compare(Raw(_data), Raw(str));
    if (_channelNumber == channleNumber && res)
        emit signalEvent();
}
