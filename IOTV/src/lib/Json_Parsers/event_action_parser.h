#pragma once

#include <forward_list>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "events/iotv_event.h"
#include "actions/iotv_action.h"

#include "base_host.h"

class Event_Action_Parser
{
public:
    Event_Action_Parser() = delete;
    ~Event_Action_Parser() = delete;

    static std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>> parseJson(const QByteArray &data, const std::forward_list<const Base_Host *> &hosts);
    static QList<QList<QVariantMap> > parseJsonToVariantMap(const QByteArray &data, const std::forward_list<const Base_Host *> &hosts);
    //    static parseVariantMapToJson();
    static QByteArray toData(const std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>> &list);

    static const Base_Host *hostByName(const std::forward_list<const Base_Host *> &hosts, const QString &name);

private:
    static IOTV_Event *parseEvent(const QJsonObject &jobj, const std::forward_list<const Base_Host *> &hosts);
    static IOTV_Action *parseAction(const QJsonObject &jobj, const std::forward_list<const Base_Host *> &hosts);

    static QJsonObject parseEvent(const IOTV_Event *event);
    static QJsonObject parseAction(const IOTV_Action *action);

    static QVariantMap parseJsonToVariantMapEvent(const IOTV_Event *event, const QString &name);
    static QVariantMap parseJsonToVariantMapAction(const IOTV_Action *action);
    //    static void writeDatatoJson(const Raw &raw, QJsonObject &id);
};

