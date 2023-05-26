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
    if (type == Json_Event_Action::TYPE_CONN)
        return new IOTV_Event_Connect(host);
    else if (type == Json_Event_Action::TYPE_DISCONN)
        return new IOTV_Event_Disconnect(host);

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

IOTV_Event *IOTV_Event_Manager::createEvent(Base_Host *host, const QString &type, const QString &direction,
                                            const QByteArray &data, const QString &dataType,
                                            const QString &compare, uint8_t channelNumber) const
{
    if (type != Json_Event_Action::TYPE_DATA)
        return nullptr;

    IOTV_Event_Data::DATA_DIRECTION newDirection = IOTV_Event_Data::DATA_DIRECTION::NONE;
    if (direction == Json_Event_Action::DIRECTION_RX)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::RX;
    else if (direction == Json_Event_Action::DIRECTION_TX)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::TX;
    else if (direction == Json_Event_Action::DIRECTION_ANY)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::ANY;

    Raw resultRaw(Raw::dataType(dataType), data);
    if (!resultRaw.isValid())
        return nullptr;

    return new IOTV_Event_Data(newDirection, createCompare(compare), host, channelNumber, resultRaw);
}

IOTV_Action *IOTV_Event_Manager::createAction(const QString &type, Base_Host *host, uint8_t ch_num,
                                              const QByteArray &data, const QString &dataType) const
{
    if (type != Json_Event_Action::TYPE_DATA_TX && type != Json_Event_Action::TYPE_DATA_RX)
        return nullptr;

    Raw resultRaw(Raw::dataType(dataType), data);
    if (!resultRaw.isValid())
        return nullptr;

    if (type == Json_Event_Action::TYPE_DATA_TX)
        return new IOTV_Action_Data_TX(host, ch_num, resultRaw);

    return nullptr;
}

IOTV_Action *IOTV_Event_Manager::createAction(const QString &type, Base_Host *dstHost, Base_Host *srcHost, uint8_t dstCh_num, uint8_t srcCh_Num) const
{
    if (type != Json_Event_Action::TYPE_DATA_TX_REF && type != Json_Event_Action::TYPE_DATA_RX_REF)
        return nullptr;

    if (type == Json_Event_Action::TYPE_DATA_TX_REF)
        return new IOTV_Action_Data_TX_Ref(dstHost, dstCh_num, srcHost, srcCh_Num);

    return nullptr;
}
