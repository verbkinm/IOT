#pragma once

#include "iotv_event.h"

#include <QTimer>

class IOTV_Event_Timer : public IOTV_Event
{
    Q_OBJECT

    Q_PROPERTY(int totalSeconds READ totalSeconds WRITE setTotalSeconds NOTIFY signalTotalSecondsChanged)
    Q_PROPERTY(int hour READ hours WRITE setHours NOTIFY signalTotalSecondsChanged)
    Q_PROPERTY(int minute READ minutes WRITE setMinutes NOTIFY signalTotalSecondsChanged)
    Q_PROPERTY(int second READ seconds WRITE setSeconds NOTIFY signalTotalSecondsChanged)

public:
    explicit IOTV_Event_Timer(int seconds, QObject *parent = nullptr);

    void setTimer(int h, int m, int s = 0);

    int hours() const;
    int minutes() const;
    int seconds() const;

    void setHours(int h);
    void setMinutes(int m);
    void setSeconds(int s);

    int totalSeconds() const;
    void setTotalSeconds(int sec);

private:
    virtual void runActions() override;
    virtual bool isValid() const override;

    QTimer _timer;

signals:
    void signalTotalSecondsChanged(int);
};

