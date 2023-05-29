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

QByteArray Event_Action_Parser::toData()
{
    QByteArray result;

    return result;
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
