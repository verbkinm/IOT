#include "device.h"

Device::Device(const QString &name, QObject *parent) : Base_Host(parent),
    _name(name), _state(false)
{
    _description = "description"; // !!!

    connect(&_autoReadInterval, &QTimer::timeout, this, &Device::slotAutoReadTimeOut);
}

QString Device::getName() const
{
    return _name;
}

bool Device::getState() const
{
    return _state;
}

int64_t Device::readData(uint8_t channelNumber)
{
//    _description = "New descriptiom";
//    notify();

    if(insertExpectedResponseRead(channelNumber))
        emit signalRead(channelNumber);

    return 0;
}

int64_t Device::writeData(uint8_t channelNumber, Raw::RAW rawData)
{
    if(insertExpectedResponseWrite(channelNumber, rawData))
        emit signalWrite(channelNumber, rawData);

    return 0;
}

void Device::dataResived(QByteArray data)
{

    notify();
}

void Device::setState(bool state)
{
    _state = state;
    notify();
}

int Device::getAutoReadInterval() const
{
    return _autoReadInterval.interval();
}

void Device::setAutoReadInterval(int value)
{
    _autoReadInterval.setInterval(value);
}

void Device::slotAutoReadTimeOut()
{
    for (uint8_t i = 0; i < readChannelLength(); i++)
        emit signalRead(i);
}
