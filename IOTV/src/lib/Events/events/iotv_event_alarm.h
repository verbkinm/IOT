#pragma once

#include "iotv_event.h"

#include <QTimer>
#include <QTime>

class IOTV_Event_Alarm : public IOTV_Event
{
    Q_OBJECT

    Q_PROPERTY(QString days READ dayString WRITE setDaysFromString NOTIFY signalDaysChanged)
    Q_PROPERTY(int hour READ getHour CONSTANT)
    Q_PROPERTY(int minute READ getMinute CONSTANT)

public:
    IOTV_Event_Alarm(const QTime &time, const std::array<bool, 7> &days, QObject *parent = nullptr);

    QTime time() const;
    bool isDayEnable(uint8_t day) const;

    QString dayString() const;
    void setDaysFromString(const QString &daysString);

    int getHour() const;
    int getMinute() const;

    const std::array<bool, 7> &days() const;

private:
    virtual void runActions() override;
    virtual bool isValid() const override;

    QTime _time;
    std::array<bool, 7> _days;
    QTimer _timer;

signals:
    void signalDaysChanged(QString newDays);
};

