#include "device.h"

Device::Device(const QString &name, QObject *parent) : Base_Host(parent),
    _name(name), _state(false)
{

}

QString Device::getName() const
{
    return _name;
}

bool Device::isConnected() const
{
    return _state;
}

int64_t Device::readData(uint8_t channelNumber)
{
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

}

void Device::setState(bool state)
{
    _state = state;
}
