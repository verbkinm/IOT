#include "event_action_parser.h"

std::forward_list<std::pair<IOTV_Event *, IOTV_Action *> > Event_Action_Parser::parseJson(const QByteArray &data, const std::forward_list<const Base_Host *> &hosts)
{
    QJsonParseError err;
    QJsonDocument jdoc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parse json " << err.errorString() << ' ' << err.offset;
        exit(-1);
    }

    std::forward_list<std::pair<IOTV_Event *, IOTV_Action *>> result;

    if (jdoc.isObject())
    {

        auto jobj = jdoc.object();
        for (const auto &el : jobj)
        {
            if (el.isObject())
            {
                IOTV_Event *event = nullptr;
                IOTV_Action *action = nullptr;

                auto id_pair = el.toObject(); // id_
                if (!id_pair.contains(Json_Event_Action::EVENT) || !id_pair.contains(Json_Event_Action::ACTION))
                {
                    qDebug() << "error json object " << id_pair;
                    continue;
                }

                auto json_event = id_pair.value(Json_Event_Action::EVENT).toObject();
                event = parseEvent(json_event, hosts);

                auto json_action = id_pair.value(Json_Event_Action::ACTION).toObject();
                action = parseAction(json_action, hosts);

                result.push_front({event, action});
            }
        }
    }

    return result;
}

QByteArray Event_Action_Parser::toData(const std::forward_list<std::pair<IOTV_Event *, IOTV_Action *>> &list)
{
    QJsonDocument jdoc;
    QJsonObject jobj_root;

    int index = 0;
    for (const auto &pair : list)
    {
        QJsonObject jpair;

        auto jevent = parseEvent(pair.first);
        auto jaction = parseAction(pair.second);

        jpair.insert("event", jevent);
        jpair.insert("action", jaction);

        jobj_root.insert("id_" + QString::number(index++), jpair);
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
        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toInt();
        QByteArray data = jobj.value(Json_Event_Action::DATA).toVariant().toByteArray();
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
        QByteArray data = jobj.value(Json_Event_Action::DATA).toVariant().toByteArray();
        QString dataType = jobj.value(Json_Event_Action::DATA_TYPE).toVariant().toString();
        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toInt();

        action = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(host), ch_num, data, dataType);

    }
    else if (type == Json_Event_Action::TYPE_DATA_TX_REF)
    {
        const Base_Host *dstHost = hostByName(hosts, jobj.value(Json_Event_Action::HOST_DST).toString());
        const Base_Host *srcHost = hostByName(hosts, jobj.value(Json_Event_Action::HOST_SRC).toString());

        uint8_t dstChNum = jobj.value(Json_Event_Action::CH_NUM_DST).toInt();
        uint8_t srcChNum = jobj.value(Json_Event_Action::CH_NUM_SRC).toInt();

        action = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(dstHost), const_cast<Base_Host *>(srcHost), dstChNum, srcChNum);
    }

    return action;
}

const Base_Host *Event_Action_Parser::hostByName(const std::forward_list<const Base_Host *> &hosts, const QString &name)
{
    auto it = std::ranges::find_if(hosts, [&name](const auto host){
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
        if (dataEv->type() == IOTV_Event_Data::DATA_DIRECTION::RX)
            direction = Json_Event_Action::DIRECTION_RX;
        else if (dataEv->type() == IOTV_Event_Data::DATA_DIRECTION::TX)
            direction = Json_Event_Action::DIRECTION_TX;
        else if (dataEv->type() == IOTV_Event_Data::DATA_DIRECTION::ANY)
            direction = Json_Event_Action::DIRECTION_ANY;
        else if (dataEv->type() == IOTV_Event_Data::DATA_DIRECTION::CHANGE)
            direction = Json_Event_Action::DIRECTION_CHANGE;

        QString compare = dataEv->compareStr();
        id.insert(Json_Event_Action::DIRECTION, direction);
        id.insert(Json_Event_Action::COMPARE, compare);
        id.insert(Json_Event_Action::CH_NUM, dataEv->channelNumber());

        if (compare != Json_Event_Action::COMPARE_ALWAYS_TRUE && compare != Json_Event_Action::COMPARE_ALWAYS_FALSE)
        {
            auto raw = dataEv->data();
            id.insert(Json_Event_Action::DATA_TYPE, raw.strData().second);

            writeDatatoJson(raw, id);
        }
    }

    return id;
}

void Event_Action_Parser::writeDatatoJson(const Raw &raw, QJsonObject &id)
{
    id.insert(Json_Event_Action::DATA_TYPE, raw.strData().second);

    if (raw.isInt())
        id.insert(Json_Event_Action::DATA, raw.strData().first.toInt());
    else if (raw.isReal())
        id.insert(Json_Event_Action::DATA, raw.strData().first.toDouble());
    else if (raw.isBool())
    {
        QString val = raw.strData().first;
        bool boolVal = true;
        if (val == "false" || val == '0')
            boolVal = false;

        id.insert(Json_Event_Action::DATA, boolVal);
    }
    else if (raw.isString())
        id.insert(Json_Event_Action::DATA, raw.strData().first);
}

QJsonObject Event_Action_Parser::parseAction(const IOTV_Action *action)
{
    QJsonObject id;
    if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DATA_TX);

        const IOTV_Action_Data_TX *actioDataTX = dynamic_cast<const IOTV_Action_Data_TX *>(action);
        Q_ASSERT(actioDataTX != nullptr);

        id.insert(Json_Event_Action::HOST_NAME, actioDataTX->host()->getName());
        id.insert(Json_Event_Action::CH_NUM, actioDataTX->channelNumber());

        auto raw = actioDataTX->data();
        writeDatatoJson(raw, id);
    }
    else if (action->type() == IOTV_Action::ACTION_TYPE::DATA_TX_REF)
    {
        id.insert(Json_Event_Action::TYPE, Json_Event_Action::TYPE_DATA_TX_REF);

        const IOTV_Action_Data_TX_Ref *actioDataTX_Ref = dynamic_cast<const IOTV_Action_Data_TX_Ref *>(action);
        Q_ASSERT(actioDataTX_Ref != nullptr);

        id.insert(Json_Event_Action::HOST_SRC, actioDataTX_Ref->srcHost()->getName());
        id.insert(Json_Event_Action::HOST_DST, actioDataTX_Ref->dstHost()->getName());

        id.insert(Json_Event_Action::CH_NUM_SRC, actioDataTX_Ref->srcChannelNumber());
        id.insert(Json_Event_Action::CH_NUM_DST, actioDataTX_Ref->dstChannelNumber());
    }

    return id;
}
