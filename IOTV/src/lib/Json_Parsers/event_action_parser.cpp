#include "event_action_parser.h"

//#include "events/iotv_event_connect.h"
//#include "events/iotv_event_disconnect.h"
#include "iotv_event_manager.h"
#include "events/iotv_event_state.h"
#include "events/iotv_event_data.h"
#include "events/iotv_event_alarm.h"
#include "events/iotv_event_timer.h"

#include "actions/iotv_action_data_tx.h"
#include "actions/iotv_action_data_tx_ref.h"
//#include "actions/iotv_action_msg.h"

//<Имя группы, <Имя события-действия, <Событие, Действие>>>
std::pair<
    std::pair<Event_List, Action_List>,
    std::pair<std::set<QString>, std::set<QString>>
    >
Event_Action_Parser::parseJson(const QByteArray &data, const std::forward_list<const Base_Host *> &hosts)
{
    QJsonParseError err;
    QJsonDocument jdoc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parse json " << err.errorString() << ' ' << err.offset;
        return {};
    }

    if (!jdoc.isObject())
        return {};

    auto jroot = jdoc.object();

    if (jroot.size() != 4)
        return {};

    // объекты для возврата из функции
    Action_List actions;
    Event_List events;
    std::set<QString> action_groups;
    std::set<QString> event_groups;

    // итераторы
    QJsonObject::Iterator it_actions;
    QJsonObject::Iterator it_events;
    QJsonObject::Iterator it_action_groups;
    QJsonObject::Iterator it_event_groups;

    it_actions = jroot.find(Json_Event_Action::ACTIONS);
    if (it_actions == jroot.end())
        return {};

    it_events = jroot.find(Json_Event_Action::EVENTS);
    if (it_events == jroot.end())
        return {};

    it_action_groups = jroot.find(Json_Event_Action::ACTION_GROUPS);
    if (it_action_groups == jroot.end())
        return {};

    it_event_groups = jroot.find(Json_Event_Action::EVENT_GROUPS);
    if (it_event_groups == jroot.end())
        return {};

    auto jactions = it_actions->toObject();
    auto jevents = it_events->toObject();
    auto jaction_groups = it_action_groups->toArray();
    auto jevent_groups = it_event_groups->toArray();

    for (const auto &jaction : jactions)
    {
        IOTV_Action *action = parseAction(jaction.toObject(), hosts);
        if (action != nullptr)
            actions.emplace(action);
    }

    for (const auto &jevent : jevents)
    {
        IOTV_Event *event = parseEvent(jevent.toObject(), hosts);
        if (event != nullptr)
            events.emplace(event);
    }

    for (const auto &jaction_group : jaction_groups)
        action_groups.insert(jaction_group.toString());

    for (const auto &jevent_group : jevent_groups)
        event_groups.insert(jevent_group.toString());

    return {{events, actions}, {event_groups, action_groups}};
}

QByteArray Event_Action_Parser::toData(const Event_List &events, const Action_List &actions,
                                       const std::set<QString> &event_groups, const std::set<QString> &action_groups)
{
    QJsonDocument jdoc;

    QJsonObject jobj_root;
    QJsonObject jactions;
    QJsonObject jevents;
    QJsonArray jevent_group;
    QJsonArray jaction_group;

    int counter_actions = 0;
    for (const auto &action : actions)
        jactions.insert(Json_Event_Action::ACTIONS + "_" + QString::number(counter_actions++), parseAction(action));

    int counter_events = 0;
    for (const auto &event : events)
        jevents.insert(Json_Event_Action::EVENTS + "_" + QString::number(counter_events++), parseEvent(event));

    for (const auto &action_group : action_groups)
        jaction_group.append(action_group);

    for (const auto &event_group : event_groups)
        jevent_group.append(event_group);

    jobj_root.insert(Json_Event_Action::ACTIONS, jactions);
    jobj_root.insert(Json_Event_Action::EVENTS, jevents);
    jobj_root.insert(Json_Event_Action::ACTION_GROUPS, jaction_group);
    jobj_root.insert(Json_Event_Action::EVENT_GROUPS, jevent_group);

    jdoc.setObject(jobj_root);

    return jdoc.toJson();
}

IOTV_Event *Event_Action_Parser::parseEvent(const QJsonObject &jobj, const std::forward_list<const Base_Host *> &hosts)
{
    QString type = jobj.value(Json_Event_Action::TYPE).toString();
    QString hostName = jobj.value(Json_Event_Action::HOST_NAME).toString();
    const Base_Host *host = hostByName(hosts, hostName);

    IOTV_Event *event = nullptr;

    if (type == Json_Event_Action::TYPE_NONE || type == Json_Event_Action::TYPE_CONN || type == Json_Event_Action::TYPE_DISCONN)
    {
        event = IOTV_Event_Manager::createEvent(host, type);
    }
    else if (type == Json_Event_Action::TYPE_STATE)
    {
        QString state = jobj.value(Json_Event_Action::TYPE_STATE).toString();
        event = IOTV_Event_Manager::createEvent(host, type, state);
    }
    else if (type == Json_Event_Action::TYPE_DATA)
    {
        QString direction = jobj.value(Json_Event_Action::DIRECTION).toString();
        QString compare = jobj.value(Json_Event_Action::COMPARE).toString();
        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toString().toInt();
        QString data = jobj.value(Json_Event_Action::DATA).toString();
        event = IOTV_Event_Manager::createEvent(host, type, direction, data, compare, ch_num);
    }
    else if (type == Json_Event_Action::TYPE_ALARM)
    {
        QString timeStr = jobj.value(Json_Event_Action::ALARM_TIME).toString();
        QString daysStr = jobj.value(Json_Event_Action::ALARM_DAYS).toString();

        QTime time;
        std::array<bool, 7> days;

        QStringList HMS = timeStr.split(':');
        if ( HMS.size() != 3 )
            return nullptr;

        bool ok_h, ok_m, ok_s;
        int h = HMS.at(0).toInt(&ok_h);
        int m = HMS.at(1).toInt(&ok_m);
        int s = HMS.at(2).toInt(&ok_s);

        if (ok_h == false || ok_m == false || ok_s == false)
            return nullptr;

        time.setHMS(h, m, s);

        if ( daysStr.size() != 7 )
            return nullptr;

        for (uint i = 0; i < days.size(); ++i)
        {
            bool ok;
            int val = QString(daysStr.at(i)).toUInt(&ok);
            if ( ok == false )
                return nullptr;
            if ( val > 1 )
                return nullptr;

            days[i] = val;
        }

        event = IOTV_Event_Manager::createEvent(type, time, days);
    }
    else if (type == Json_Event_Action::TYPE_TIMER)
    {
        QString seconds = jobj.value(Json_Event_Action::TIMER_SECONDS).toString();
        bool ok;
        int sec = seconds.toInt(&ok);
        if (ok)
            event = IOTV_Event_Manager::createEvent(type, sec);
        else
            event = nullptr;
    }

    if (event != nullptr)
    {
        QString group(jobj.value(Json_Event_Action::EVENT_GROUP).toString());
        QString name(jobj.value(Json_Event_Action::EVENT_NAME).toString());
        QString enable = jobj.value(Json_Event_Action::EVENT_ENABLE).toString();

        event->setGroup(group);
        event->setName(name);
        event->setEnable(enable == "true" ? true : false);

        event->setHostName(hostName);

        auto jactions = jobj.value(Json_Event_Action::ACTIONS);

        // json: "actions": [ [ "", [""] ], [ "", [""] ] ]
        auto arr_1_level = jactions.toArray(); // - [ [ "группа 1", ["действие 1", "действие 2"] ], [ "группа 2", ["действие 1", "действие 2"] ] ]
        for (auto groupAndActions : arr_1_level)
        {
            auto arr_2_level = groupAndActions.toArray(); // - [ "группа 1", ["действие 1", "действие 2"] ]
            if (arr_2_level.size() == 2)
            {
                QString groupName(arr_2_level.at(0).toString());
                std::set<QString> actionName;
                auto arr_3_level = arr_2_level.at(1).toArray(); // - ["действие 1", "действие 2"]
                for (auto action : arr_3_level)
                    actionName.insert(action.toString());

                event->actionMustBeenBinding[groupName] = actionName;
            }
        }
    }

    return event;
}

IOTV_Action *Event_Action_Parser::parseAction(const QJsonObject &jobj, const std::forward_list<const Base_Host *> &hosts)
{
    QString type = jobj.value(Json_Event_Action::TYPE).toString();
    IOTV_Action *action = nullptr;

    if (type == Json_Event_Action::TYPE_DATA_TX)
    {
        QString hostName = jobj.value(Json_Event_Action::HOST_NAME).toString();
        const Base_Host *host = hostByName(hosts, hostName);
        QString data = jobj.value(Json_Event_Action::DATA).toString();
        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toString().toInt();

        action = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(host), ch_num, data);
        static_cast<IOTV_Action_Data_TX *>(action)->setHostName(hostName);

    }
    else if (type == Json_Event_Action::TYPE_DATA_TX_REF)
    {
        QString dstHostName = jobj.value(Json_Event_Action::HOST_DST).toString();
        QString srcHostName = jobj.value(Json_Event_Action::HOST_SRC).toString();

        const Base_Host *dstHost = hostByName(hosts, dstHostName);
        const Base_Host *srcHost = hostByName(hosts, srcHostName);

        uint8_t dstChNum = jobj.value(Json_Event_Action::CH_NUM_DST).toString().toInt();
        uint8_t srcChNum = jobj.value(Json_Event_Action::CH_NUM_SRC).toString().toInt();

        action = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(dstHost), const_cast<Base_Host *>(srcHost), dstChNum, srcChNum);
        auto tx_ref_action = static_cast<IOTV_Action_Data_TX_Ref *>(action);
        tx_ref_action->setDstHostName(dstHostName);
        tx_ref_action->setSrcHostName(srcHostName);
    }
    else
        action = IOTV_Event_Manager::createAction();

    if (action != nullptr)
    {
        QString group(jobj.value(Json_Event_Action::ACTION_GROUP).toString());
        QString name(jobj.value(Json_Event_Action::ACTION_NAME).toString());
        QString enable = jobj.value(Json_Event_Action::ACTION_ENABLE).toString();

        action->setGroup(group);
        action->setName(name);
        action->setEnable(enable == "true" ? true : false);
    }

    return action;
}

const Base_Host *Event_Action_Parser::hostByName(const std::forward_list<const Base_Host *> &hosts, const QString &name)
{
    auto it = std::find_if (hosts.begin(), hosts.end(), [&name](auto &host){
        return host->getName() == name;
    });

    if (it != hosts.end())
        return *it;

    return nullptr;
}

void Event_Action_Parser::toDataRunAction(const QString &groupName, const QString &actionName)
{
    QJsonDocument jdoc;

    QJsonObject jobj_root;
//    QJsonObject jgroup, jactions;

    jobj_root.insert(Json_Event_Action::ACTION_GROUP, groupName);
    jobj_root.insert(Json_Event_Action::ACTION_NAME, actionName);

    jdoc.setObject(jobj_root);

    qDebug() << jobj_root;
}

QJsonObject Event_Action_Parser::parseEvent(std::shared_ptr<IOTV_Event> event)
{
    QJsonObject id;
    if (event->type() == IOTV_Event::EVENT_TYPE::NONE)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::DATA_TYPE_NONE);
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::CONNECTING)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_CONN);
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::DISCONNECTING)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DISCONN);
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::STATE)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_STATE);

        const IOTV_Event_State *state = dynamic_cast<const IOTV_Event_State *>(event.get());
        if (state == nullptr)
            return {};

        QString stateStr = Json_Event_Action::STATE_NONE;
        if (state->state() == IOTV_Event_State::STATE_TYPE::ONLINE)
            stateStr = Json_Event_Action::STATE_ONLINE;
        else if (state->state() == IOTV_Event_State::STATE_TYPE::OFFLINE)
            stateStr = Json_Event_Action::STATE_OFFNLINE;
        else if (state->state() == IOTV_Event_State::STATE_TYPE::SWITCH)
            stateStr = Json_Event_Action::STATE_SWITCH;

        id.insert(Json_Event_Action::STATE, stateStr);
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::DATA)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DATA);

        const IOTV_Event_Data *dataEv = dynamic_cast<const IOTV_Event_Data *>(event.get());
        if (dataEv == nullptr)
            return {};

        QString direction = Json_Event_Action::DIRECTION_NONE;
        if (dataEv->direction() == IOTV_Event_Data::DATA_DIRECTION::RX)
            direction = Json_Event_Action::DIRECTION_RX;
        else if (dataEv->direction() == IOTV_Event_Data::DATA_DIRECTION::TX)
            direction = Json_Event_Action::DIRECTION_TX;
        else if (dataEv->direction() == IOTV_Event_Data::DATA_DIRECTION::ANY)
            direction = Json_Event_Action::DIRECTION_ANY;
        else if (dataEv->direction() == IOTV_Event_Data::DATA_DIRECTION::CHANGE)
            direction = Json_Event_Action::DIRECTION_CHANGE;

        QString compare = dataEv->compareStr();

        id.insert(Json_Event_Action::DIRECTION, direction);
        id.insert(Json_Event_Action::COMPARE, compare);
        id.insert(Json_Event_Action::CH_NUM, QString::number(dataEv->channelNumber()));

        if (compare != Json_Event_Action::COMPARE_ALWAYS_TRUE && compare != Json_Event_Action::COMPARE_ALWAYS_FALSE)
            id.insert(Json_Event_Action::DATA, dataEv->data());
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::ALARM)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_ALARM);

        const IOTV_Event_Alarm *alarmEvent = dynamic_cast<const IOTV_Event_Alarm *>(event.get());
        if (alarmEvent == nullptr)
            return {};

        id.insert(Json_Event_Action::ALARM_TIME, alarmEvent->time().toString("h:m:s"));
        id.insert(Json_Event_Action::ALARM_DAYS, alarmEvent->dayString());
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::TIMER)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_TIMER);

        const IOTV_Event_Timer *timerEvent = dynamic_cast<const IOTV_Event_Timer *>(event.get());
        if (timerEvent == nullptr)
            return {};
        
        id.insert(Json_Event_Action::TIMER_SECONDS, QString::number(timerEvent->totalSeconds()));
    }

    if (event->type() != IOTV_Event::EVENT_TYPE::NONE
        && event->type() != IOTV_Event::EVENT_TYPE::ALARM
        && event->type() != IOTV_Event::EVENT_TYPE::TIMER)
    {
        id.insert(Json_Event_Action::HOST_NAME, event->hostName());
    }

    id.insert(Json_Event_Action::EVENT_NAME, event->name());
    id.insert(Json_Event_Action::EVENT_GROUP, event->group());
    id.insert(Json_Event_Action::EVENT_ENABLE, event->isEnable() == true ? "true" : "false");


    QJsonArray jaction_lvl_1; // - [ [ "группа 1", ["действие 1", "действие 2"] ], [ "группа 2", ["действие 1", "действие 2"] ], ... ]
    QJsonArray jaction_lvl_2; // - [ "группа 1", ["действие 1", "действие 2"] ]
    QJsonArray jaction_lvl_3; // - ["действие 1", "действие 2"]

    for (const auto &pair : event->actionMustBeenBinding)
    {
        QString groupName = pair.first;
        QStringList actionList(pair.second.begin(), pair.second.end());

        jaction_lvl_3 = QJsonArray::fromStringList(actionList);
        QJsonValue jGroupName(groupName);
        jaction_lvl_2.append(jGroupName);
        jaction_lvl_2.append(jaction_lvl_3);

        jaction_lvl_1.append(jaction_lvl_2);
    }

    id.insert(Json_Event_Action::ACTIONS, jaction_lvl_1);

    return id;
}

QJsonObject Event_Action_Parser::parseAction(std::shared_ptr<IOTV_Action> action)
{
    if (action == nullptr)
        return {};

    QJsonObject id;

    if (action->type() == IOTV_Action::ACTION_TYPE::NONE)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::DATA_TYPE_NONE);
    }
    else if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DATA_TX);

        const IOTV_Action_Data_TX *actioDataTX = dynamic_cast<const IOTV_Action_Data_TX *>(action.get());
        if (actioDataTX == nullptr)
            return {};

        id.insert(Json_Event_Action::HOST_NAME, actioDataTX->hostName());
        id.insert(Json_Event_Action::CH_NUM, QString::number(actioDataTX->channelNumber()));

        id.insert(Json_Event_Action::DATA, actioDataTX->data());
    }
    else if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX_REF)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DATA_TX_REF);

        const IOTV_Action_Data_TX_Ref *actioDataTX_Ref = dynamic_cast<const IOTV_Action_Data_TX_Ref *>(action.get());
        if (actioDataTX_Ref == nullptr)
            return {};

        id.insert(Json_Event_Action::HOST_SRC, actioDataTX_Ref->srcHostName());
        id.insert(Json_Event_Action::HOST_DST, actioDataTX_Ref->dstHostName());

        id.insert(Json_Event_Action::CH_NUM_SRC, QString::number(actioDataTX_Ref->srcChannelNumber()));
        id.insert(Json_Event_Action::CH_NUM_DST, QString::number(actioDataTX_Ref->dstChannelNumber()));
    }

    id.insert(Json_Event_Action::ACTION_NAME, action->name());
    id.insert(Json_Event_Action::ACTION_GROUP, action->group());
    id.insert(Json_Event_Action::ACTION_ENABLE, action->isEnable() == true ? "true" : "false");

    return id;
}
