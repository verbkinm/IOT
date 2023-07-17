#pragma once

#include "iotv_event.h"

#include <QTimer>

class IOTV_Event_Timer : public IOTV_Event
{
public:
    IOTV_Event_Timer(int seconds, QObject *parent = nullptr);

    int seconds() const;

private:
    QTimer _timer;
};

