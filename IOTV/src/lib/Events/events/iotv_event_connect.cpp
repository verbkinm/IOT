#include "iotv_event_connect.h"

IOTV_Event_Connect::IOTV_Event_Connect(const Base_Host *host, QObject *parent) :
    IOTV_Event(EVENT_TYPE::CONNECTING, host, parent)
{
    if (host == nullptr)
        return;

    connect(host, &Base_Host::signalConnected, this, &IOTV_Event::signalEvent, Qt::QueuedConnection);
}
