#pragma once

#include "iotv_event.h"

#include <QTimer>
#include <QTime>

class IOTV_Event_Alarm : public IOTV_Event
{
public:
    IOTV_Event_Alarm(const QTime &time, const std::array<bool, 7> &days, QObject *parent = nullptr);

    QTime time() const;
    bool isDayEnable(uint8_t day) const;
    QString dayString() const;

private:
    QTime _time;
    std::array<bool, 7> _days;
    QTimer _timer;

private slots:
    void slotTimeCheck();
};

