#include "iotv_event_manager.h"

#include "event_action_parser.h"
#include "events/iotv_event_connect.h"
#include "events/iotv_event_disconnect.h"
#include "events/iotv_event_state.h"
#include "events/iotv_event_data.h"
#include "events/iotv_event_alarm.h"
#include "events/iotv_event_timer.h"

#include "actions/iotv_action_data_tx.h"
#include "actions/iotv_action_data_tx_ref.h"
//#include "actions/iotv_action_msg.h"

IOTV_Event_Manager::IOTV_Event_Manager(const Event_List &events, const Action_List &actions,
                                       const std::set<QString> &event_groups, const std::set<QString> &action_groups)
    : _events(events), _actions(actions), _event_groups(event_groups), _action_groups(action_groups)
{
    // Если в списке событий (_events)  есть название группы, которое отсутствует в списке групп событий (_event_groups),
    // добавляем эту группу в список групп событий (_event_groups)
    for (const auto &event : _events)
        _event_groups.insert(event->group());

    // Тоже самое для действий и групп действий
    for (const auto &action : _actions)
        _action_groups.insert(action->group());


    for (const auto &event : _events)
    {
        for (auto &pair : event->actionMustBeenBinding)
        {
            QString actionGroupName = pair.first;
            _action_groups.insert(actionGroupName);

            for (const auto &actionName : pair.second)
            {
                if (findAction(actionGroupName, actionName) == nullptr)
                {
                    std::shared_ptr<IOTV_Action> ptr(createAction());
                    ptr->setName(actionName);
                    ptr->setGroup(actionGroupName);
                    _actions.insert(ptr);
                }
            }
        }
    }
}

IOTV_Event_Manager::~IOTV_Event_Manager()
{

}

void IOTV_Event_Manager::bind()
{
    for (auto &event : _events)
    {
        event->clearActions();
        auto &actionMustBeenBinding = event->actionMustBeenBinding;
        for (auto &pair : actionMustBeenBinding)
        {
            QString groupName = pair.first;
            for (auto &actionName : pair.second)
                event->addAction(findAction(groupName, actionName));
        }
    }
}

IOTV_Event *IOTV_Event_Manager::createEvent(const Base_Host *host, const QString &type)
{
    if (type == Json_Event_Action::TYPE_NONE)
        return new IOTV_Event(IOTV_Event::EVENT_TYPE::NONE, host);
    else if (type == Json_Event_Action::TYPE_CONN)
        return new IOTV_Event_Connect(host);
    else if (type == Json_Event_Action::TYPE_DISCONN)
        return new IOTV_Event_Disconnect(host);

    return nullptr;
}

IOTV_Event *IOTV_Event_Manager::createEvent(const Base_Host *host, const QString &type, const QString &state)
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

IOTV_Event *IOTV_Event_Manager::createEvent(const Base_Host *host, const QString &type, const QString &direction,
                                            const QString &data,
                                            const QString &compare, uint8_t channelNumber)
{
    if (type != Json_Event_Action::TYPE_DATA)
        return nullptr;

    QString cmp = compare;

    IOTV_Event_Data::DATA_DIRECTION newDirection = IOTV_Event_Data::DATA_DIRECTION::NONE;
    if (direction == Json_Event_Action::DIRECTION_RX)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::RX;
    else if (direction == Json_Event_Action::DIRECTION_TX)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::TX;
    else if (direction == Json_Event_Action::DIRECTION_ANY)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::ANY;
    else if (direction == Json_Event_Action::DIRECTION_CHANGE)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::CHANGE;

    if (cmp != Json_Event_Action::COMPARE_EQ && cmp != Json_Event_Action::COMPARE_NE
        && cmp != Json_Event_Action::COMPARE_GR && cmp != Json_Event_Action::COMPARE_LS
        && cmp != Json_Event_Action::COMPARE_GE && cmp != Json_Event_Action::COMPARE_LE
        && cmp != Json_Event_Action::COMPARE_ALWAYS_TRUE && cmp != Json_Event_Action::COMPARE_ALWAYS_FALSE)
    {
        cmp = Json_Event_Action::COMPARE_ALWAYS_FALSE;
    }

    return new IOTV_Event_Data(newDirection, cmp, host, channelNumber, data);
}

IOTV_Event *IOTV_Event_Manager::createEvent(const QString &type, const QTime &time, const std::array<bool, 7> &days)
{
    if (type != Json_Event_Action::TYPE_ALARM)
        return nullptr;

    return new IOTV_Event_Alarm(time, days);
}

IOTV_Event *IOTV_Event_Manager::createEvent(const QString &type, int seconds)
{
    if (type != Json_Event_Action::TYPE_TIMER)
        return nullptr;

    return new IOTV_Event_Timer(seconds);
}

IOTV_Action *IOTV_Event_Manager::createAction()
{
    return new IOTV_Action(IOTV_Action::ACTION_TYPE::NONE);
}

//IOTV_Event *IOTV_Event_Manager::createEvent(const QVariantMap &event, const std::forward_list<const Base_Host *> &hosts)
//{
//    IOTV_Event *result = nullptr;

//    const Base_Host *host = Event_Action_Parser::hostByName(hosts, event[Json_Event_Action::HOST_NAME].toString());

//    if (event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_CONN || event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_DISCONN)
//    {
//        QString type = event[Json_Event_Action::TYPE].toString();
//        result = createEvent(host, type);
//    }
//    else if (event[Json_Event_Action::TYPE] == Json_Event_Action::STATE)
//    {
//        QString state = event[Json_Event_Action::STATE].toString();
//        QString type = event[Json_Event_Action::TYPE].toString();

//        result = IOTV_Event_Manager::createEvent(host, type, state);
//    }
//    else if(event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_DATA)
//    {
//        QString direction = event[Json_Event_Action::DIRECTION].toString();
//        QString compare = event[Json_Event_Action::COMPARE].toString();
//        uint8_t ch_num = event[Json_Event_Action::CH_NUM].toString().toInt();
//        QString data = event[Json_Event_Action::DATA].toString();
//        QString type = event[Json_Event_Action::TYPE].toString();

//        result = IOTV_Event_Manager::createEvent(host, type, direction, data, compare, ch_num);
//    }
//    else if(event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_ALARM)
//    {
//        QString timeStr = event[Json_Event_Action::ALARM_TIME].toString();
//        QString daysStr = event[Json_Event_Action::ALARM_DAYS].toString();
//        QString type = event[Json_Event_Action::TYPE].toString();

//        QTime time;
//        std::array<bool, 7> days;

//        QStringList HM = timeStr.split(':');
//        if ( HM.size() != 2 )
//            return nullptr;

//        bool ok_h, ok_m;
//        int h = HM.at(0).toInt(&ok_h);
//        int m = HM.at(1).toInt(&ok_m);

//        if (ok_h == false || ok_m == false)
//            return nullptr;

//        time.setHMS(h, m, 0);

//        if ( daysStr.size() != 7 )
//            return nullptr;

//        for (uint i = 0; i < days.size(); ++i)
//        {
//            auto ch = daysStr.at(i);
//            if ( ch != '0' && ch != '1' )
//                return nullptr;

//            days[i] = QString(ch).toInt();
//        }
//        result = IOTV_Event_Manager::createEvent(type, time, days);
//    }
//    else if(event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_TIMER)
//    {
//        QString seconds = event[Json_Event_Action::TIMER_SECONDS].toString();
//        QString type = event[Json_Event_Action::TYPE].toString();

//        bool ok;
//        int sec = seconds.toInt(&ok);

//        if (ok)
//            result = IOTV_Event_Manager::createEvent(type, sec);
//        else
//            result = nullptr;
//    }

//    return result;
//}

IOTV_Action *IOTV_Event_Manager::createAction(const QString &type, Base_Host *host, uint8_t ch_num,
                                              const QString &data)
{
    if (type == Json_Event_Action::TYPE_DATA_TX)
        return new IOTV_Action_Data_TX(host, ch_num, data);

    return nullptr;
}

IOTV_Action *IOTV_Event_Manager::createAction(const QString &type, Base_Host *dstHost, Base_Host *srcHost, uint8_t dstCh_num, uint8_t srcCh_Num)
{
    if (type != Json_Event_Action::TYPE_DATA_TX_REF)
        return nullptr;

    return new IOTV_Action_Data_TX_Ref(dstHost, dstCh_num, srcHost, srcCh_Num);
}

const Event_List &IOTV_Event_Manager::events() const
{
    return _events;
}

const Action_List &IOTV_Event_Manager::actions() const
{
    return _actions;
}

Event_List IOTV_Event_Manager::eventsInGroup(const QString &groupName) const
{
    Event_List list;
    for (auto &event : _events)
    {
        if (event == nullptr)
            continue;

        if (event->group() == groupName)
            list.insert(event);
    }

    return list;
}

Action_List IOTV_Event_Manager::actionsInGroup(const QString &groupName) const
{
    Action_List list;
    for (auto &action : _actions)
    {
        if (action == nullptr)
            continue;

        if (action->group() == groupName)
            list.insert(action);
    }

    return list;
}

void IOTV_Event_Manager::addEvent(std::shared_ptr<IOTV_Event> event)
{
    if (event != nullptr)
        _events.insert(event);
}

void IOTV_Event_Manager::addAction(std::shared_ptr<IOTV_Action> action)
{
    if (action != nullptr)
        _actions.insert(action);
}

std::shared_ptr<IOTV_Event> IOTV_Event_Manager::findEvent(const QString &groupName, const QString &eventName) const
{
    for (auto &event : _events)
    {
        if (event != nullptr)
        {
            if ( event->group() == groupName && event->name() == eventName)
                return event;
        }
    }

    return nullptr;
}

std::shared_ptr<IOTV_Action> IOTV_Event_Manager::findAction(const QString &groupName, const QString &eventName) const
{
    for (auto &action : _actions)
    {
        if (action != nullptr)
        {
            if ( action->group() == groupName && action->name() == eventName)
                return action;
        }
    }

    return nullptr;
}

void IOTV_Event_Manager::deleteEvent(const QString &groupName, const QString &eventName)
{
    for (auto &event : _events)
    {
        if (event == nullptr)
            continue;

        if (event->group() == groupName && event->name() == eventName)
        {
            _events.erase(event);
            break;
        }
    }
}

void IOTV_Event_Manager::deleteAction(const QString &groupName, const QString &actionName)
{
    for (auto &action : _actions)
    {
        if (action == nullptr)
            continue;

        if (action->group() == groupName && action->name() == actionName)
        {
            _actions.erase(action);
            break;
        }
    }
}

void IOTV_Event_Manager::runEvent(const QString &groupName, const QString &eventName)
{
    auto event = findEvent(groupName, eventName);

    if (event != nullptr)
        event->forceExec();
}

void IOTV_Event_Manager::runAction(const QString &groupName, const QString &actionName)
{
    auto action = findAction(groupName, actionName);

    if (action != nullptr)
        action->forceExec();
}

const std::set<QString> &IOTV_Event_Manager::eventGroups() const
{
    return _event_groups;
}

const std::set<QString> &IOTV_Event_Manager::actionGroups() const
{
    return _action_groups;
}

void IOTV_Event_Manager::addEventGroup(const QString &groupName)
{
    _event_groups.insert(groupName);
}

void IOTV_Event_Manager::addActionGroup(const QString &groupName)
{
    _action_groups.insert(groupName);
}

void IOTV_Event_Manager::removeEventGroup(const QString &groupName)
{
    _event_groups.erase(groupName);
}

void IOTV_Event_Manager::removeActionGroup(const QString &groupName)
{
    _action_groups.erase(groupName);
}

void IOTV_Event_Manager::renameEventGroup(const QString &oldGroupName, const QString &newGroupName)
{
    std::set<QString>::iterator it = _event_groups.find(oldGroupName);
    if (it != _event_groups.end())
    {
        _event_groups.erase(it);
        _event_groups.insert(newGroupName);
    }

    for (auto &event : _events)
    {
        if (event == nullptr)
            continue;

        if (event.get()->group() == oldGroupName)
            event.get()->setGroup(newGroupName);
    }
}

void IOTV_Event_Manager::renameActionGroup(const QString &oldGroupName, const QString &newGroupName)
{
    std::set<QString>::iterator it = _action_groups.find(oldGroupName);
    if (it != _action_groups.end())
    {
        _action_groups.erase(it);
        _action_groups.insert(newGroupName);
    }

    for (auto &action : _actions)
    {
        if (action == nullptr)
            continue;
        if (action.get()->group() == oldGroupName)
            action.get()->setGroup(newGroupName);
    }

    for (auto &event : _events)
    {
        event->actionMustBeenBinding[newGroupName] = event->actionMustBeenBinding[oldGroupName];
        event->actionMustBeenBinding.erase(oldGroupName);

        if (event->actionMustBeenBinding[newGroupName].size() == 0)
            event->actionMustBeenBinding.erase(newGroupName);
    }
}

void IOTV_Event_Manager::clear()
{
    _events.clear();
    _actions.clear();

    _event_groups.clear();
    _action_groups.clear();
}

IOTV_Event *IOTV_Event_Manager::copyEvent(const IOTV_Event *event)
{
    if (event == nullptr)
        return nullptr;

    IOTV_Event *result = nullptr;
    IOTV_Event::EVENT_TYPE type = event->type();

    if (type == IOTV_Event::EVENT_TYPE::NONE)
        result = new IOTV_Event(IOTV_Event::EVENT_TYPE::NONE, nullptr);
    else if (type == IOTV_Event::EVENT_TYPE::CONNECTING)
        result = new IOTV_Event_Connect(event->host());
    else if (type == IOTV_Event::EVENT_TYPE::DISCONNECTING)
        result = new IOTV_Event_Disconnect(event->host());
    else if (type == IOTV_Event::EVENT_TYPE::STATE)
    {
        auto state = static_cast<const IOTV_Event_State *>(event)->state();
        result = new IOTV_Event_State(state, event->host());
    }
    else if (type == IOTV_Event::EVENT_TYPE::DATA)
    {
        const IOTV_Event_Data *event_data = static_cast<const IOTV_Event_Data *>(event);
        if (event_data != nullptr)
        {
            auto direction = event_data->direction();
            auto compare = event_data->compareStr();
            uint8_t chNum = event_data->channelNumber();
            QString data = event_data->data();

            result = new IOTV_Event_Data(direction, compare, event->host(), chNum, data);
        }
    }
    else if (type == IOTV_Event::EVENT_TYPE::ALARM)
    {
        const IOTV_Event_Alarm *event_alarm = static_cast<const IOTV_Event_Alarm *>(event);
        if (event_alarm != nullptr)
            result = new IOTV_Event_Alarm(event_alarm->time(), event_alarm->days());
    }
    else if (type == IOTV_Event::EVENT_TYPE::TIMER)
    {
        const IOTV_Event_Timer *event_timer = static_cast<const IOTV_Event_Timer *>(event);
        if (event_timer != nullptr)
            result = new IOTV_Event_Timer(event_timer->totalSeconds());
    }

    if (result != nullptr)
    {
        result->setName(event->name());
        result->setGroup(event->group());
        result->setEnable(event->isEnable());
        result->setHostName(event->hostName());
        result->actionMustBeenBinding = event->actionMustBeenBinding;
    }

    return result;
}

IOTV_Action *IOTV_Event_Manager::copyAction(const IOTV_Action *action)
{
    if (action == nullptr)
        return nullptr;

    IOTV_Action *result = nullptr;
    IOTV_Action::ACTION_TYPE type = action->type();

    if (type == IOTV_Action::ACTION_TYPE::NONE)
        result = new IOTV_Action(IOTV_Action::ACTION_TYPE::NONE, nullptr);
    else if (type == IOTV_Action::ACTION_TYPE::DATA_TX)
    {
        const IOTV_Action_Data_TX *tx = dynamic_cast<const IOTV_Action_Data_TX *>(action);
        if (tx == nullptr)
            return nullptr;

        Base_Host *host = const_cast<Base_Host *>(tx->host());
        IOTV_Action_Data_TX *result_data_tx = new IOTV_Action_Data_TX(host, tx->channelNumber(), tx->data());
        result_data_tx->setHostName(tx->hostName());
        result = result_data_tx;
    }
    else if (type == IOTV_Action::ACTION_TYPE::DATA_TX_REF)
    {
        const IOTV_Action_Data_TX_Ref *tx_ref = dynamic_cast<const IOTV_Action_Data_TX_Ref *>(action);
        if (tx_ref == nullptr)
            return nullptr;

        Base_Host *dstHost = const_cast<Base_Host *>(tx_ref->dstHost());
        Base_Host *srcHost = const_cast<Base_Host *>(tx_ref->srcHost());

        IOTV_Action_Data_TX_Ref *result_data_tx_ref = new IOTV_Action_Data_TX_Ref(dstHost, tx_ref->dstChannelNumber(),
                                                                                  srcHost, tx_ref->srcChannelNumber());

        result_data_tx_ref->setSrcHostName(tx_ref->srcHostName());
        result_data_tx_ref->setDstHostName(tx_ref->dstHostName());

        result = result_data_tx_ref;
    }

    if (result != nullptr)
    {
        result->setName(action->name());
        result->setGroup(action->group());
        result->setEnable(action->isEnable());
    }

    return result;
}

std::set<QString> IOTV_Event_Manager::allHostsName() const
{
    std::set<QString> result;

    for (const auto &el : _events)
    {
        if (el != nullptr)
            result.insert(el->hostName());
    }

    for (const auto &el : _actions)
    {
        if (el == nullptr)
            continue;

        if (el->type() == IOTV_Action::ACTION_TYPE::DATA_TX)
        {
            const IOTV_Action_Data_TX *action = dynamic_cast<const IOTV_Action_Data_TX *>(el.get());
            if (action != nullptr)
                result.insert(action->hostName());
        }
        else if (el->type() == IOTV_Action::ACTION_TYPE::DATA_TX_REF)
        {
            const IOTV_Action_Data_TX_Ref *action = dynamic_cast<const IOTV_Action_Data_TX_Ref *>(el.get());
            if (action != nullptr)
            {
                result.insert(action->srcHostName());
                result.insert(action->dstHostName());
            }
        }
    }

    return result;
}
