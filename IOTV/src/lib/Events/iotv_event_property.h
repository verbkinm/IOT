#pragma once

#include "events/iotv_event_connect.h"
#include "events/iotv_event_disconnect.h"
#include "events/iotv_event_state.h"
#include "events/iotv_event_data.h"

class IOTV_Event_property
{
public:
    IOTV_Event_property() = delete;

    static QString strProperty(IOTV_Event *event, const QString &propertyString);

private:
    static QString type(const IOTV_Event::EVENT_TYPE &t);
};
