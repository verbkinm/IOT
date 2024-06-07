#include "iotv_event_alarm.h"

IOTV_Event_Alarm::IOTV_Event_Alarm(const QTime &time, const std::array<bool, 7> &days, QObject *parent)
    : IOTV_Event{EVENT_TYPE::ALARM, nullptr, parent}, _time(time), _days(days)
{
    connect(&_timer, &QTimer::timeout, this, &IOTV_Event_Alarm::runActions);
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

int IOTV_Event_Alarm::getHour() const
{
    return _time.hour();
}

int IOTV_Event_Alarm::getMinute() const
{
    return _time.minute();
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
    if (type() != EVENT_TYPE::ALARM)
        return false;

    return true;
}
