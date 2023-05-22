#include "iotv_event_state.h"

IOTV_Event_State::IOTV_Event_State(STATE_TYPE state, const Base_Host *host, QObject *parent) :
    IOTV_Event(EVENT_TYPE::STATE, host, parent)
{
    if (host == nullptr)
        return;

    if (state == STATE_TYPE::ONLINE)
        connect(host, &Base_Host::signalStateOnline, this, &IOTV_Event::signalEvent, Qt::UniqueConnection);
    else if (state == STATE_TYPE::OFFLINE)
        connect(host, &Base_Host::signalStateOffline, this, &IOTV_Event::signalEvent, Qt::UniqueConnection);
    else if (state == STATE_TYPE::SWITCH)
        connect(host, &Base_Host::signalStateChanged, this, &IOTV_Event::signalEvent, Qt::UniqueConnection);
}

IOTV_Event_State::STATE_TYPE IOTV_Event_State::state() const
{
    return _state;
}
