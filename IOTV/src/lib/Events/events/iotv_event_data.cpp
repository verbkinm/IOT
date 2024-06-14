#include "iotv_event_data.h"

IOTV_Event_Data::IOTV_Event_Data(const DATA_DIRECTION &direction, const QString &compare,
                                 const Base_Host *host,
                                 uint8_t channelNumber, const QString &data,
                                 QObject *parent) :
    IOTV_Event(EVENT_TYPE::DATA, host, parent),
    _type(direction),
    _channelNumber(channelNumber),
    _data(data),
    _compareStr(compare),
    _compare(createCompare(compare))
{
    if (host == nullptr)
        return;

    if (direction == DATA_DIRECTION::RX)
        connect(host, &Base_Host::signalDataRX, this, &IOTV_Event_Data::slotCheckData, Qt::QueuedConnection);
    else if (direction == DATA_DIRECTION::TX)
        connect(host, &Base_Host::signalDataTX, this, &IOTV_Event_Data::slotCheckData, Qt::QueuedConnection);
    else if (direction == DATA_DIRECTION::ANY)
    {
        connect(host, &Base_Host::signalDataRX, this, &IOTV_Event_Data::slotCheckData, Qt::QueuedConnection);
        connect(host, &Base_Host::signalDataTX, this, &IOTV_Event_Data::slotCheckData, Qt::QueuedConnection);
    }
    else if (direction == DATA_DIRECTION::CHANGE)
        connect(host, &Base_Host::signalDataChanged, this, &IOTV_Event_Data::slotCheckData, Qt::QueuedConnection);
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

QString IOTV_Event_Data::compareStr() const
{
    return _compareStr;
}

void IOTV_Event_Data::setCompareStr(const QString &newCompare)
{
    if (_compareStr == newCompare)
        return;

    _compareStr = newCompare;
    _compare = createCompare(newCompare);

    emit signalCompareChanged(newCompare);
}

void IOTV_Event_Data::setDataStr(const QString &newDataStr)
{
    if (_data == newDataStr)
        return;

    _data = newDataStr;
    emit signalDataChanged(newDataStr);
}

void IOTV_Event_Data::setChannelNumber(uint8_t newChNum)
{
    if (_channelNumber == newChNum)
        return;

    _channelNumber = newChNum;
    emit signalChannelNumberChanged(newChNum);
}

uint8_t IOTV_Event_Data::channelNumber() const
{
    return _channelNumber;
}

QString IOTV_Event_Data::data() const
{
    return _data;
}

QString IOTV_Event_Data::getDirection() const
{
    int dir = static_cast<int>(direction());
    return directionType[dir];
}

void IOTV_Event_Data::setDirection(const QString &newDirection)
{
    DATA_DIRECTION old_state = _type;

    if (newDirection == directionType[1])
        _type = DATA_DIRECTION::RX;
    else if (newDirection == directionType[2])
        _type = DATA_DIRECTION::TX;
    else if (newDirection == directionType[3])
        _type = DATA_DIRECTION::ANY;
    else if (newDirection == directionType[4])
        _type = DATA_DIRECTION::CHANGE;
    else
        _type = DATA_DIRECTION::NONE;

    if (_type != old_state)
        emit signalDirectionChanged(newDirection);
}

void IOTV_Event_Data::runActions()
{
    execActions();
}

bool IOTV_Event_Data::isValid() const
{
    if (host() == nullptr || host()->getId() == 0 || type() != EVENT_TYPE::DATA)
        return false;

    return true;
}

void IOTV_Event_Data::slotCheckData(uint8_t channleNumber, QByteArray rhs)
{
    if (!isValid())
        return;

    Raw rawHost(_host->getReadChannelType(channleNumber), rhs);
    QString str = rawHost.strData().first;
    bool res = _compare(Raw(_data), Raw(str));

    if (_channelNumber == channleNumber && res)
        runActions();
}
