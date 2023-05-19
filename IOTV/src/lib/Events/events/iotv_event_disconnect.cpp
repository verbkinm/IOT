#include "iotv_event_disconnect.h"

IOTV_Event_Disconnect::IOTV_Event_Disconnect(const Base_Host *host, QObject *parent) :
    IOTV_Event(EVENT_TYPE::DISCONNECTING, host, parent)
{
    if (host != nullptr)
        connect(host, &Base_Host::signalDisconnected, this, &IOTV_Event::signalEvent, Qt::UniqueConnection);
}
