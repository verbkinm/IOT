#include "shedule.h"

Shedule::Shedule(char RW, uint8_t channelNumber, int interval, const QString& fileName, QObject *parent) : QObject(parent),
    RW(RW), channelNumber(channelNumber), fileName(fileName)
{
    connect(&_timer, &QTimer::timeout, this, &Shedule::signalTimeOut);
    _timer.start(interval);
}
