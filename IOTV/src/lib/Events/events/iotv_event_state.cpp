#include "iotv_event_state.h"

IOTV_Event_State::IOTV_Event_State(STATE_TYPE state, const Base_Host *host, QObject *parent) :
    IOTV_Event(EVENT_TYPE::STATE, host, parent),
    _state(state)
{
    if (host == nullptr)
        return;

    if (state == STATE_TYPE::ONLINE)
        connect(host, &Base_Host::signalStateOnline, this, &IOTV_Event_State::runActions, Qt::QueuedConnection);
    else if (state == STATE_TYPE::OFFLINE)
        connect(host, &Base_Host::signalStateOffline, this, &IOTV_Event_State::runActions, Qt::QueuedConnection);
    else if (state == STATE_TYPE::SWITCH)
        connect(host, &Base_Host::signalStateChanged, this, &IOTV_Event_State::runActions, Qt::QueuedConnection);
}

IOTV_Event_State::STATE_TYPE IOTV_Event_State::state() const
{
    return _state;
}

QString IOTV_Event_State::getState() const
{
    return stateName[static_cast<int>(state())];
}

void IOTV_Event_State::setState(const QString &state)
{
    STATE_TYPE old_state = _state;

    if (state == stateName[1])
        _state = STATE_TYPE::ONLINE;
    else if (state == stateName[2])
        _state = STATE_TYPE::OFFLINE;
    else if (state == stateName[3])
        _state = STATE_TYPE::SWITCH;
    else
        _state = STATE_TYPE::NONE;

    if (_state != old_state)
        emit signalStateSchanged(state);
}

void IOTV_Event_State::runActions()
{
    if (!isValid())
        return;

    execActions();
}

bool IOTV_Event_State::isValid() const
{
    if (host() == nullptr || type() != EVENT_TYPE::STATE)
        return false;

    return true;
}
