#include "event_action_parser.h"

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

    if (host == nullptr)
        return event;

    if (type == Json_Event_Action::TYPE_CONN || type == Json_Event_Action::TYPE_DISCONN)
    {
        event = IOTV_Event_Manager::createEvent(const_cast<Base_Host *>(host), type);
    }
    else if (type == Json_Event_Action::TYPE_STATE)
    {
        QString state = jobj.value(Json_Event_Action::TYPE_STATE).toString();
        event = IOTV_Event_Manager::createEvent(const_cast<Base_Host *>(host), type, state);
    }
    else if (type == Json_Event_Action::TYPE_DATA)
    {
        QString direction = jobj.value(Json_Event_Action::DIRECTION).toString();
        QString compare = jobj.value(Json_Event_Action::COMPARE).toString();
        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toString().toInt();
        QString data = jobj.value(Json_Event_Action::DATA).toString();
        QString dataType = jobj.value(Json_Event_Action::DATA_TYPE).toString();
        event = IOTV_Event_Manager::createEvent(const_cast<Base_Host *>(host), type, direction, data, dataType, compare, ch_num);
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
        QString dataType = jobj.value(Json_Event_Action::DATA_TYPE).toString();
        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toString().toInt();

        action = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(host), ch_num, data, dataType);

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
    auto it = std::find_if (hosts.begin(), hosts.end(), [&name](const auto host){
        return host->getName() == name;
    });

    if (it != hosts.end())
        return *it;

    return nullptr;
}

QJsonObject Event_Action_Parser::parseEvent(const IOTV_Event *event)
{
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
        {
            auto raw = dataEv->data();
            id.insert(Json_Event_Action::DATA_TYPE, raw.strData().second);
            id.insert(Json_Event_Action::DATA, raw.strData().first);
        }
    }

    return id;
}

//void Event_Action_Parser::writeDatatoJson(const Raw &raw, QJsonObject &id)
//{
////    id.insert(Json_Event_Action::DATA_TYPE, raw.strData().second);

////    if (raw.isInt())
////        id.insert(Json_Event_Action::DATA, raw.strData().first.toInt());
////    else if (raw.isReal())
////        id.insert(Json_Event_Action::DATA, raw.strData().first.toDouble());
////    else if (raw.isBool())
////    {
////        QString val = raw.strData().first;
////        bool boolVal = true;
////        if (val == "false" || val == '0')
////            boolVal = false;

////        id.insert(Json_Event_Action::DATA, boolVal);
////    }
////    else if (raw.isString())
////        id.insert(Json_Event_Action::DATA, raw.strData().first);
//}

QJsonObject Event_Action_Parser::parseAction(const IOTV_Action *action)
{
    QJsonObject id;
    if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DATA_TX);

        const IOTV_Action_Data_TX *actioDataTX = dynamic_cast<const IOTV_Action_Data_TX *>(action);
        Q_ASSERT(actioDataTX != nullptr);

        id.insert(Json_Event_Action::HOST_NAME, actioDataTX->host()->getName());
        id.insert(Json_Event_Action::CH_NUM, QString::number(actioDataTX->channelNumber()));

        auto raw = actioDataTX->data();
        id.insert(Json_Event_Action::DATA_TYPE, raw.strData().second);
        id.insert(Json_Event_Action::DATA, raw.strData().first);
//        writeDatatoJson(raw, id);
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
        {
            newEvent[Json_Event_Action::DATA_TYPE] = eventData->data().strData().second;
            newEvent[Json_Event_Action::DATA] = eventData->data().strData().first;
        }
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
        newAction[Json_Event_Action::DATA] = dataTX->data().strData().first;
        newAction[Json_Event_Action::DATA_TYPE] = dataTX->data().strData().second;
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
