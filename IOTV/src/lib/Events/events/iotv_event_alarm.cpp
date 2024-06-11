#include "iotv_event_alarm.h"

IOTV_Event_Alarm::IOTV_Event_Alarm(const QTime &time, const std::array<bool, 7> &days, QObject *parent)
    : IOTV_Event{EVENT_TYPE::ALARM, nullptr, parent}, _time(time), _days(days)
{
    connect(&_timer, &QTimer::timeout, this, &IOTV_Event_Alarm::runActions, Qt::QueuedConnection);
    _timer.start(1000);
}

QTime IOTV_Event_Alarm::time() const
{
    return _time;
}

bool IOTV_Event_Alarm::isDayEnable(uint8_t day) const
{
    if ( day >= _days.size() )
        return false;

    return _days[day];
}

QString IOTV_Event_Alarm::dayString() const
{
    QString result;
    for (uint i = 0; i < _days.size(); ++i)
        result += QString::number(_days[i]);

    return result;
}

void IOTV_Event_Alarm::setDaysFromString(const QString &daysString)
{
    if (size_t(daysString.size()) != _days.size())
        return;

    if (dayString() == daysString)
        return;

    for (size_t i = 0; i < _days.size(); ++i)
        _days[i] = daysString[i] == '1' ? true : false;

    emit signalDaysChanged(daysString);
}

int IOTV_Event_Alarm::hours() const
{
    return _time.hour();
}

int IOTV_Event_Alarm::minutes() const
{
    return _time.minute();
}

int IOTV_Event_Alarm::seconds() const
{
    return _time.second();
}

void IOTV_Event_Alarm::setHours(int h)
{
    _time.setHMS(h, _time.minute(), _time.second());

    emit signalHoursChanged(h);
}

void IOTV_Event_Alarm::setMinutes(int m)
{
    _time.setHMS(_time.hour(), m, _time.second());
    emit signalMinutesChanged(m);
}

void IOTV_Event_Alarm::setSeconds(int s)
{
    _time.setHMS(_time.hour(), _time.minute(), s);
    emit signalSecondsChanged(s);
}

int IOTV_Event_Alarm::totalSeconds() const
{
    return _time.msecsSinceStartOfDay() / 1000;
}

void IOTV_Event_Alarm::setTotalSeconds(int sec)
{
    _time = QTime::fromMSecsSinceStartOfDay(sec * 1000);
    emit signalTotalSecondsChanged(sec);
}

const std::array<bool, 7> &IOTV_Event_Alarm::days() const
{
    return _days;
}

void IOTV_Event_Alarm::runActions()
{
    if (!isValid())
        return;

    if (_days[QDate::currentDate().dayOfWeek() - 1] && QTime::currentTime().toString("h:m:s") == _time.toString("h:m:s"))
        execActions();
}

bool IOTV_Event_Alarm::isValid() const
{
    if (type() != EVENT_TYPE::ALARM || !_time.isValid())
        return false;

    return true;
}
