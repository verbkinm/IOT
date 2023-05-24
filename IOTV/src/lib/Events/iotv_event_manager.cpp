#include "iotv_event_manager.h"

IOTV_Event_Manager::IOTV_Event_Manager(QObject *parent)
    : QObject{parent}
{

}

bool IOTV_Event_Manager::bind(IOTV_Event *event, IOTV_Action *action)
{
    if (event == nullptr && action == nullptr)
        return false;
    if (event == nullptr && action != nullptr)
    {
        delete action;
        return false;
    }
    if (event != nullptr && action == nullptr)
    {
        delete event;
        return false;
    }

    if (!event->isValid() || !action->isValid())
    {
        delete event;
        delete action;
        return false;
    }

    event->setParent(this);
    action->setParent(this);

    connect(event, &IOTV_Event::signalEvent, action, &IOTV_Action::exec, Qt::UniqueConnection);
    _worker.emplace_back(event, action);

    return true;
}

const std::list<std::pair<IOTV_Event *, IOTV_Action *> > &IOTV_Event_Manager::worker() const
{
    return _worker;
}

size_t IOTV_Event_Manager::size() const
{
    return _worker.size();
}

IOTV_Event *IOTV_Event_Manager::createEvent(Base_Host *host, const QString &type) const
{
    if (type == Json_Event_Action::TYPE_CONN) return new IOTV_Event_Connect(host);
    else if (type == Json_Event_Action::TYPE_DISCONN) return new IOTV_Event_Disconnect(host);

    return nullptr;
}

IOTV_Event *IOTV_Event_Manager::createEvent(Base_Host *host, const QString &type, const QString &state) const
{
    if (type != Json_Event_Action::TYPE_STATE)
        return nullptr;

    IOTV_Event_State::STATE_TYPE newState = IOTV_Event_State::STATE_TYPE::NONE;
    if (state == Json_Event_Action::STATE_ONLINE)
        newState = IOTV_Event_State::STATE_TYPE::ONLINE;
    else if (state == Json_Event_Action::STATE_OFFNLINE)
        newState = IOTV_Event_State::STATE_TYPE::OFFLINE;
    else if (state == Json_Event_Action::STATE_SWITCH)
        newState = IOTV_Event_State::STATE_TYPE::SWITCH;

    return new IOTV_Event_State(newState, host);
}

std::function<bool(Raw, Raw)> IOTV_Event_Manager::createCompare(const QString &compare) const
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


    return [](Raw, Raw)->bool
    {
        return false;
    };
}

IOTV_Event *IOTV_Event_Manager::createEvent(Base_Host *host, const QString &type, const QString &direction,
                                            const QVariant &variant, const QString &compare, uint8_t channelNumber) const
{
    if (type != Json_Event_Action::TYPE_DATA)
        return nullptr;

    IOTV_Event_Data::DATA_DIRECTION newDirection = IOTV_Event_Data::DATA_DIRECTION::NONE;
    if (direction == Json_Event_Action::DIRECTION_RX) newDirection = IOTV_Event_Data::DATA_DIRECTION::RX;
    else if (direction == Json_Event_Action::DIRECTION_TX) newDirection = IOTV_Event_Data::DATA_DIRECTION::TX;

    Raw resultRaw;
    Raw::DATA_TYPE rawType = host->getReadChannelType(channelNumber);

    bool ok;
    if (rawType == Raw::DATA_TYPE::BOOL)
    {
        bool val = variant.toBool();
        resultRaw = Raw(val);
    }
    else if (rawType == Raw::DATA_TYPE::INT_8)
    {
        int8_t val = variant.toInt(&ok);
        if (!ok)
            return nullptr;

        resultRaw = Raw(val);
    }
    else if (rawType == Raw::DATA_TYPE::INT_16)
    {
        int16_t val = variant.toInt(&ok);
        if (!ok)
            return nullptr;

        resultRaw = Raw(val);
    }
    else if (rawType == Raw::DATA_TYPE::INT_32)
    {
        int32_t val = variant.toInt(&ok);
        if (!ok)
            return nullptr;

        resultRaw = Raw(val);
    }
    else if (rawType == Raw::DATA_TYPE::INT_64)
    {
        int64_t val = variant.toLongLong(&ok);
        if (!ok)
            return nullptr;

        resultRaw = Raw(val);
    }
    else if (rawType == Raw::DATA_TYPE::FLOAT_32)
    {
        float val = variant.toFloat(&ok);
        if (!ok)
            return nullptr;

        resultRaw = Raw(val);
    }
    else if (rawType == Raw::DATA_TYPE::DOUBLE_64)
    {
        double val = variant.toDouble(&ok);
        if (!ok)
            return nullptr;

        resultRaw = Raw(val);
    }
    else if (rawType == Raw::DATA_TYPE::STRING)
    {
        QString val = variant.toString();
        resultRaw = Raw(val);
    }
    else if (rawType == Raw::DATA_TYPE::RAW || rawType == Raw::DATA_TYPE::NONE)
    {
        resultRaw = Raw(variant.toByteArray());
    }

    return new IOTV_Event_Data(newDirection, createCompare(compare), host, channelNumber, resultRaw);
}
