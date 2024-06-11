#pragma once

#include "iotv_event.h"

#include <QTimer>
#include <QTime>

class IOTV_Event_Alarm : public IOTV_Event
{
    Q_OBJECT

    Q_PROPERTY(int totalSeconds READ totalSeconds WRITE setTotalSeconds NOTIFY signalTotalSecondsChanged)
    Q_PROPERTY(QString days READ dayString WRITE setDaysFromString NOTIFY signalDaysChanged)
    Q_PROPERTY(int hour READ hours WRITE setHours NOTIFY signalHoursChanged)
    Q_PROPERTY(int minute READ minutes WRITE setMinutes NOTIFY signalMinutesChanged)
    Q_PROPERTY(int second READ seconds WRITE setSeconds NOTIFY signalSecondsChanged)


public:
    IOTV_Event_Alarm(const QTime &time, const std::array<bool, 7> &days, QObject *parent = nullptr);

    QTime time() const;
    bool isDayEnable(uint8_t day) const;

    QString dayString() const;
    void setDaysFromString(const QString &daysString);

    int hours() const;
    int minutes() const;
    int seconds() const;

    void setHours(int h);
    void setMinutes(int m);
    void setSeconds(int s);

    int totalSeconds() const;
    void setTotalSeconds(int sec);

    const std::array<bool, 7> &days() const;

private:
    virtual void runActions() override;
    virtual bool isValid() const override;

    QTime _time;
    std::array<bool, 7> _days;
    QTimer _timer;

signals:
    void signalDaysChanged(QString newDays);

    void signalHoursChanged(int);
    void signalMinutesChanged(int);
    void signalSecondsChanged(int);
    void signalTotalSecondsChanged(int);
};

