#include "iotv_event_property.h"

QString IOTV_Event_property::strProperty(IOTV_Event *event, const QString &propertyString)
{
    if (event == nullptr)
        return "";

    if (propertyString == Json_Event_Action::TYPE)
    {
        return type(event->type());
    }



}

QString IOTV_Event_property::type(const IOTV_Event::EVENT_TYPE &t)
{
    switch (t) {
    case IOTV_Event::EVENT_TYPE::CONNECTING:
        return Json_Event_Action::TYPE_CONN;
        break;
    case IOTV_Event::EVENT_TYPE::DISCONNECTING:
        return Json_Event_Action::TYPE_CONN;
        break;
    case IOTV_Event::EVENT_TYPE::STATE:
        return Json_Event_Action::TYPE_STATE;
        break;
    case IOTV_Event::EVENT_TYPE::DATA:
        return Json_Event_Action::TYPE_DATA;
        break;
    default:
        return "NONE";
        break;
    }
}
