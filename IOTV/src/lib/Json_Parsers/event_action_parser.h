#pragma once

#include <forward_list>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <set>

#include "events/iotv_event.h"
#include "actions/iotv_action.h"

class Event_Action_Parser
{
public:
    Event_Action_Parser() = delete;
    ~Event_Action_Parser() = delete;

    static std::pair<
        std::pair<std::vector<std::shared_ptr<IOTV_Event>>, std::vector<std::shared_ptr<IOTV_Action>>>,
        std::pair<std::set<QString>, std::set<QString>>
        >
    parseJson(const QByteArray &data, const std::forward_list<const Base_Host *> &hosts);

    static QByteArray toData(const std::vector<std::shared_ptr<IOTV_Event>> &events, const std::vector<std::shared_ptr<IOTV_Action>> &actions,
                             const std::set<QString> &event_groups, const std::set<QString> &action_groups);


    static const Base_Host *hostByName(const std::forward_list<const Base_Host *> &hosts, const QString &name);

private:
    static IOTV_Event *parseEvent(const QJsonObject &jobj, const std::forward_list<const Base_Host *> &hosts);
    static IOTV_Action *parseAction(const QJsonObject &jobj, const std::forward_list<const Base_Host *> &hosts);

    static QJsonObject parseEvent(std::shared_ptr<IOTV_Event> event);
    static QJsonObject parseAction(std::shared_ptr<IOTV_Action> action);
};

