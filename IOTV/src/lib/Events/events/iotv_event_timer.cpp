#include "iotv_event_timer.h"

IOTV_Event_Timer::IOTV_Event_Timer(int seconds, QObject *parent)
    : IOTV_Event{EVENT_TYPE::TIMER, nullptr, parent}
{
    connect(&_timer, &QTimer::timeout, this, &IOTV_Event::signalEvent);
    _timer.start(seconds * 1000);
}

int IOTV_Event_Timer::seconds() const
{
    return _timer.interval() / 1000;
}
