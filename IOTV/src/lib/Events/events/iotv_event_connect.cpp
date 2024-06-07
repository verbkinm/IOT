#include "iotv_event_connect.h"

IOTV_Event_Connect::IOTV_Event_Connect(const Base_Host *host, QObject *parent) :
    IOTV_Event(EVENT_TYPE::CONNECTING, host, parent)
{
    if (host != nullptr)
        connect(host, &Base_Host::signalConnected, this, &IOTV_Event_Connect::runActions, Qt::QueuedConnection);
}

void IOTV_Event_Connect::runActions()
{
    if (!isValid())
        return;

    execActions();
}

bool IOTV_Event_Connect::isValid() const
{
    if (_host == nullptr || type() != EVENT_TYPE::CONNECTING)
        return false;

    return true;
}
