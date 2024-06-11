#include "iotv_event_timer.h"

IOTV_Event_Timer::IOTV_Event_Timer(int seconds, QObject *parent)
    : IOTV_Event{EVENT_TYPE::TIMER, nullptr, parent}
{
    if (seconds < 10)
        seconds = 10;

    connect(&_timer, &QTimer::timeout, this, &IOTV_Event_Timer::runActions, Qt::QueuedConnection);
    _timer.start(seconds * 1000);
}

void IOTV_Event_Timer::setTimer(int h, int m, int s)
{
    setTotalSeconds(h * 3600 + m * 60 + s);
}

int IOTV_Event_Timer::hours() const
{
    return totalSeconds() / 3600;
}

int IOTV_Event_Timer::minutes() const
{
    return totalSeconds() / 60 - hours() * 60;
}

int IOTV_Event_Timer::seconds() const
{
    return totalSeconds() - minutes() * 60 - hours() * 3600;
}

void IOTV_Event_Timer::setHours(int h)
{
    setTimer(h, minutes(), seconds());
}

void IOTV_Event_Timer::setMinutes(int m)
{
    setTimer(hours(), m, seconds());
}

void IOTV_Event_Timer::setSeconds(int s)
{
    setTimer(hours(), minutes(), s);
}

int IOTV_Event_Timer::totalSeconds() const
{
    return _timer.interval() / 1000;
}

void IOTV_Event_Timer::setTotalSeconds(int sec)
{
    _timer.start(sec * 1000);
    emit signalTotalSecondsChanged(sec);
}

void IOTV_Event_Timer::runActions()
{
    if (!isValid())
        return;

    execActions();
}

bool IOTV_Event_Timer::isValid() const
{
    if (type() != EVENT_TYPE::TIMER)
        return false;

    return true;
}
