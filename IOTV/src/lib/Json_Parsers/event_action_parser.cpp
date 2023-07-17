#include "event_action_parser.h"

#include "iotv_event_manager.h"
//#include "events/iotv_event_connect.h"
//#include "events/iotv_event_disconnect.h"
#include "events/iotv_event_state.h"
#include "events/iotv_event_data.h"
#include "events/iotv_event_alarm.h"
#include "events/iotv_event_timer.h"

#include "actions/iotv_action_data_tx.h"
#include "actions/iotv_action_data_tx_ref.h"
//#include "actions/iotv_action_msg.h"

std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>> Event_Action_Parser::parseJson(const QByteArray &data, const std::forward_list<const Base_Host *> &hosts)
{
    QJsonParseError err;
    QJsonDocument jdoc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parse json " << err.errorString() << ' ' << err.offset;
        exit(-1);
    }

    std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>> result;

    if (jdoc.isObject())
    {
        auto jobj = jdoc.object();

        auto names = jobj.keys();
        int index = 0;

        for (const auto &el : jobj)
        {
            if (el.isObject())
            {
                IOTV_Event *event = nullptr;
                IOTV_Action *action = nullptr;

                auto id_pair = el.toObject(); // Пара event и action
                if (!id_pair.contains(Json_Event_Action::EVENT) || !id_pair.contains(Json_Event_Action::ACTION))
                {
                    qDebug() << "error json object " << id_pair;
                    continue;
                }

                auto json_event = id_pair.value(Json_Event_Action::EVENT).toObject();
                event = parseEvent(json_event, hosts);

                auto json_action = id_pair.value(Json_Event_Action::ACTION).toObject();
                action = parseAction(json_action, hosts);

                result.emplace_front(names.at(index++), std::make_pair(event, action));
            }
        }
    }

    return result;
}

QList<QList<QVariantMap>> Event_Action_Parser::parseJsonToVariantMap(const QByteArray &data, const std::forward_list<const Base_Host *> &hosts)
{
    auto list = parseJson(data, hosts);

    // first in sublist QVariantMap - event
    // second in sublist QVariantMap - action
    QList<QList<QVariantMap>> result;

    for (const auto &pair : list)
    {
        auto name = pair.first;
        auto event = pair.second.first;
        auto action = pair.second.second;

        QVariantMap newEvent = parseJsonToVariantMapEvent(event, name);
        QVariantMap newAction = parseJsonToVariantMapAction(action);

        if (newEvent.isEmpty() || newAction.isEmpty())
            continue;

        QList<QVariantMap> l;
        l << newEvent << newAction;
        result << l;
    }

    return result;
}

QByteArray Event_Action_Parser::toData(const std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>> &list)
{
    QJsonDocument jdoc;
    QJsonObject jobj_root;

    for (const auto &pairs : list)
    {
        QJsonObject jpair;

        auto name = pairs.first;
        auto jevent = parseEvent(pairs.second.first);
        auto jaction = parseAction(pairs.second.second);

        jpair.insert("event", jevent);
        jpair.insert("action", jaction);

        jobj_root.insert(name, jpair);
    }

    jdoc.setObject(jobj_root);
    return jdoc.toJson();
}

IOTV_Event *Event_Action_Parser::parseEvent(const QJsonObject &jobj, const std::forward_list<const Base_Host *> &hosts)
{
    QString type = jobj.value(Json_Event_Action::TYPE).toString();
    const Base_Host *host = hostByName(hosts, jobj.value(Json_Event_Action::HOST_NAME).toString());
    IOTV_Event *event = nullptr;

    if (type == Json_Event_Action::TYPE_CONN || type == Json_Event_Action::TYPE_DISCONN)
    {
        if (host == nullptr)
            return event;

        event = IOTV_Event_Manager::createEvent(host, type);
    }
    else if (type == Json_Event_Action::TYPE_STATE)
    {
        if (host == nullptr)
            return event;

        QString state = jobj.value(Json_Event_Action::TYPE_STATE).toString();
        event = IOTV_Event_Manager::createEvent(host, type, state);
    }
    else if (type == Json_Event_Action::TYPE_DATA)
    {
        if (host == nullptr)
            return event;

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

        QStringList HM = timeStr.split(':');
        if ( HM.size() != 2 )
            return nullptr;

        bool ok_h, ok_m;
        int h = HM.at(0).toInt(&ok_h);
        int m = HM.at(1).toInt(&ok_m);

        if (ok_h == false || ok_m == false)
            return nullptr;

        time.setHMS(h, m, 0);

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

    return event;
}

IOTV_Action *Event_Action_Parser::parseAction(const QJsonObject &jobj, const std::forward_list<const Base_Host *> &hosts)
{
    QString type = jobj.value(Json_Event_Action::TYPE).toString();
    IOTV_Action *action = nullptr;

    if (type == Json_Event_Action::TYPE_DATA_TX)
    {
        const Base_Host *host = hostByName(hosts, jobj.value(Json_Event_Action::HOST_NAME).toString());
        QString data = jobj.value(Json_Event_Action::DATA).toString();
        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toString().toInt();

        action = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(host), ch_num, data);

    }
    else if (type == Json_Event_Action::TYPE_DATA_TX_REF)
    {
        const Base_Host *dstHost = hostByName(hosts, jobj.value(Json_Event_Action::HOST_DST).toString());
        const Base_Host *srcHost = hostByName(hosts, jobj.value(Json_Event_Action::HOST_SRC).toString());

        uint8_t dstChNum = jobj.value(Json_Event_Action::CH_NUM_DST).toString().toInt();
        uint8_t srcChNum = jobj.value(Json_Event_Action::CH_NUM_SRC).toString().toInt();

        action = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(dstHost), const_cast<Base_Host *>(srcHost), dstChNum, srcChNum);
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

QJsonObject Event_Action_Parser::parseEvent(const IOTV_Event *event)
{
    if (event == nullptr)
        return {};

    QJsonObject id;
    if (event->type() == IOTV_Event::EVENT_TYPE::CONNECTING)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_CONN);
        id.insert(Json_Event_Action::HOST_NAME, event->host()->getName());
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::DISCONNECTING)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DISCONN);
        id.insert(Json_Event_Action::HOST_NAME, event->host()->getName());
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::STATE)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_STATE);
        id.insert(Json_Event_Action::HOST_NAME, event->host()->getName());

        const IOTV_Event_State *state = dynamic_cast<const IOTV_Event_State *>(event);
        Q_ASSERT(state != nullptr);

        QString stateStr;
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
        id.insert(Json_Event_Action::HOST_NAME, event->host()->getName());

        const IOTV_Event_Data *dataEv = dynamic_cast<const IOTV_Event_Data *>(event);
        Q_ASSERT(dataEv != nullptr);

        QString direction;
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

        const IOTV_Event_Alarm *alarmEvent = dynamic_cast<const IOTV_Event_Alarm *>(event);
        Q_ASSERT(alarmEvent != nullptr);

        id.insert(Json_Event_Action::ALARM_TIME, alarmEvent->time().toString("h:m"));
        id.insert(Json_Event_Action::ALARM_DAYS, alarmEvent->dayString());
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::TIMER)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_TIMER);

        const IOTV_Event_Timer *timerEvent = dynamic_cast<const IOTV_Event_Timer *>(event);
        Q_ASSERT(timerEvent != nullptr);

        id.insert(Json_Event_Action::TIMER_SECONDS, QString::number(timerEvent->seconds()));
    }

    return id;
}

QJsonObject Event_Action_Parser::parseAction(const IOTV_Action *action)
{
    if (action == nullptr)
        return {};

    QJsonObject id;
    if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DATA_TX);

        const IOTV_Action_Data_TX *actioDataTX = dynamic_cast<const IOTV_Action_Data_TX *>(action);
        Q_ASSERT(actioDataTX != nullptr);

        id.insert(Json_Event_Action::HOST_NAME, actioDataTX->host()->getName());
        id.insert(Json_Event_Action::CH_NUM, QString::number(actioDataTX->channelNumber()));

        id.insert(Json_Event_Action::DATA, actioDataTX->data());
    }
    else if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX_REF)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DATA_TX_REF);

        const IOTV_Action_Data_TX_Ref *actioDataTX_Ref = dynamic_cast<const IOTV_Action_Data_TX_Ref *>(action);
        Q_ASSERT(actioDataTX_Ref != nullptr);

        id.insert(Json_Event_Action::HOST_SRC, actioDataTX_Ref->srcHost()->getName());
        id.insert(Json_Event_Action::HOST_DST, actioDataTX_Ref->dstHost()->getName());

        id.insert(Json_Event_Action::CH_NUM_SRC, QString::number(actioDataTX_Ref->srcChannelNumber()));
        id.insert(Json_Event_Action::CH_NUM_DST, QString::number(actioDataTX_Ref->dstChannelNumber()));
    }

    return id;
}

QVariantMap Event_Action_Parser::parseJsonToVariantMapEvent(const IOTV_Event *event, const QString &name)
{
    if (event == nullptr)
        return {};

    QVariantMap newEvent;

    newEvent[Json_Event_Action::EVENT_ACTION_NAME] = name;

    if ( event->host() != nullptr )
        newEvent[Json_Event_Action::HOST_NAME] = event->host()->getName();

    if (event->type() == IOTV_Event::EVENT_TYPE::CONNECTING)
    {
        newEvent[Json_Event_Action::TYPE] = Json_Event_Action::TYPE_CONN;
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::DISCONNECTING)
    {
        newEvent[Json_Event_Action::TYPE] = Json_Event_Action::TYPE_DISCONN;
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::STATE)
    {
        newEvent[Json_Event_Action::TYPE] = Json_Event_Action::TYPE_STATE;
        const IOTV_Event_State *eventState = dynamic_cast<const IOTV_Event_State *>(event);
        if (eventState == nullptr)
            return {};

        QString state;
        switch (eventState->state())
        {
        case IOTV_Event_State::STATE_TYPE::ONLINE:
            state = Json_Event_Action::STATE_ONLINE;
            break;
        case IOTV_Event_State::STATE_TYPE::OFFLINE:
            state = Json_Event_Action::STATE_OFFNLINE;
            break;
        case IOTV_Event_State::STATE_TYPE::SWITCH:
            state = Json_Event_Action::STATE_SWITCH;
            break;
        default:
            break;
        }
        newEvent[Json_Event_Action::STATE] = state;
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::DATA)
    {
        newEvent[Json_Event_Action::TYPE] = Json_Event_Action::TYPE_DATA;
        const IOTV_Event_Data *eventData = dynamic_cast<const IOTV_Event_Data *>(event);
        if (eventData == nullptr)
            return {};

        QString direction;
        switch(eventData->direction())
        {
        case IOTV_Event_Data::DATA_DIRECTION::RX:
            direction = Json_Event_Action::DIRECTION_RX;
            break;
        case IOTV_Event_Data::DATA_DIRECTION::TX:
            direction = Json_Event_Action::DIRECTION_TX;
            break;
        case IOTV_Event_Data::DATA_DIRECTION::CHANGE:
            direction = Json_Event_Action::DIRECTION_CHANGE;
            break;
        case IOTV_Event_Data::DATA_DIRECTION::ANY:
            direction = Json_Event_Action::DIRECTION_ANY;
            break;
        default:
            break;
        }
        newEvent[Json_Event_Action::CH_NUM] = eventData->channelNumber();
        QString compare = eventData->compareStr();
        newEvent[Json_Event_Action::DIRECTION] = direction;
        newEvent[Json_Event_Action::COMPARE] = compare;

        if (compare != Json_Event_Action::COMPARE_ALWAYS_TRUE && compare != Json_Event_Action::COMPARE_ALWAYS_FALSE)
            newEvent[Json_Event_Action::DATA] = eventData->data();
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::ALARM)
    {
        const IOTV_Event_Alarm *alarmEvent = dynamic_cast<const IOTV_Event_Alarm *>(event);
        if (alarmEvent == nullptr)
            return {};

        newEvent[Json_Event_Action::TYPE] = Json_Event_Action::TYPE_ALARM;
        newEvent[Json_Event_Action::ALARM_TIME] = alarmEvent->time().toString("h:m");
        newEvent[Json_Event_Action::ALARM_DAYS] = alarmEvent->dayString();
    }
    else if (event->type() == IOTV_Event::EVENT_TYPE::TIMER)
    {
        const IOTV_Event_Timer *timerEvent = dynamic_cast<const IOTV_Event_Timer *>(event);
        if (timerEvent == nullptr)
            return {};

        newEvent[Json_Event_Action::TYPE] = Json_Event_Action::TYPE_TIMER;
        newEvent[Json_Event_Action::TIMER_SECONDS] = QString::number(timerEvent->seconds());
    }

    return newEvent;
}

QVariantMap Event_Action_Parser::parseJsonToVariantMapAction(const IOTV_Action *action)
{
    if (action == nullptr)
        return {};

    QVariantMap newAction;

    if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX)
    {
        newAction[Json_Event_Action::TYPE] = Json_Event_Action::TYPE_DATA_TX;
        const IOTV_Action_Data_TX *dataTX = dynamic_cast<const IOTV_Action_Data_TX *>(action);
        if (dataTX == nullptr)
            return {};

        newAction[Json_Event_Action::HOST_NAME] = dataTX->host()->getName();
        newAction[Json_Event_Action::DATA] = dataTX->data();
        newAction[Json_Event_Action::CH_NUM] = dataTX->channelNumber();

    }
    else if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX_REF)
    {
        const IOTV_Action_Data_TX_Ref *dataTX_Ref = dynamic_cast<const IOTV_Action_Data_TX_Ref *>(action);
        if (dataTX_Ref == nullptr)
            return {};

        newAction[Json_Event_Action::TYPE] = Json_Event_Action::TYPE_DATA_TX_REF;

        newAction[Json_Event_Action::HOST_SRC] = dataTX_Ref->srcHost()->getName();
        newAction[Json_Event_Action::HOST_DST] = dataTX_Ref->dstHost()->getName();

        newAction[Json_Event_Action::CH_NUM_SRC] = dataTX_Ref->srcChannelNumber();
        newAction[Json_Event_Action::CH_NUM_DST] = dataTX_Ref->dstChannelNumber();
    }

    return newAction;
}
