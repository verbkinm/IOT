#include "device.h"

Device::Device(const IOTV_SC::DEV_PKG &dev, QObject *parent)
    : Base_Host{dev.id, parent}, _name{dev.name}, _state{false},
      _timerReadInterval{1000}, _timerStateInterval{1000}
{
    this->setDescription(dev.description);

    for (const auto &type : dev.readChannel)
        this->addReadSubChannel({type});

    for (const auto &type : dev.writeChannel)
        this->addWriteSubChannel({type});

    connect(&_timerRead, &QTimer::timeout, this, &Device::signalQueryRead);
    connect(&_timerState, &QTimer::timeout, this, &Device::signalQueryState);

    _timerRead.setInterval(_timerReadInterval);
    _timerState.setInterval(_timerStateInterval);

    _timerRead.start();
    _timerState.start();
}

QString Device::getName() const
{
    return _name;
}

bool Device::isOnline() const
{
    return _state;
}

void Device::setState(bool newState)
{
    _state = newState;
}

bool Device::setData(uint8_t channelNumber, const QByteArray &data)
{
    emit signalDataChanged(channelNumber, data);
    return this->setReadChannelData(channelNumber, data);
}

void Device::setReadInterval(int interval)
{
    _timerRead.setInterval(interval);
}
