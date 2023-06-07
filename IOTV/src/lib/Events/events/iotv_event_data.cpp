#include "iotv_event_data.h"

IOTV_Event_Data::IOTV_Event_Data(const DATA_DIRECTION &direction, QString compare,
                                 const Base_Host *host,
                                 uint8_t channelNumber, const Raw &raw,
                                 QObject *parent) :
    IOTV_Event(EVENT_TYPE::DATA, host, parent),
    _type(direction),
    _channelNumber(channelNumber),
    _data(raw),
    _compare(createCompare(compare)),
    _compareStr(compare)
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

std::function<bool(Raw, Raw)> IOTV_Event_Data::createCompare(const QString &compare)
{
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

const Raw &IOTV_Event_Data::data() const
{
    return _data;
}

QString IOTV_Event_Data::directionString() const
{
    switch (_type) {
    case DATA_DIRECTION::RX:
        return Json_Event_Action::DIRECTION_RX;
        break;
    case DATA_DIRECTION::TX:
        return Json_Event_Action::DIRECTION_TX;
        break;
    case DATA_DIRECTION::ANY:
        return Json_Event_Action::DIRECTION_ANY;
        break;
    case DATA_DIRECTION::CHANGE:
        return Json_Event_Action::DIRECTION_CHANGE;
        break;
    default:
        return "NONE";
        break;
    }
}

void IOTV_Event_Data::slotCheckData(uint8_t channleNumber, QByteArray rhs)
{
    Raw rawHost(_host->getReadChannelType(channleNumber), rhs);
    if (_channelNumber == channleNumber && _compare(_data, rawHost))
        emit signalEvent();
}
