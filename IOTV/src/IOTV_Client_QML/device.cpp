#include "device.h"

Device::Device(const IOTV_SC::DEV_PKG &dev, QObject *parent)
    : Base_Host{dev.id, parent}, _name{dev.name}, _state{false},
      _timerReadInterval{1000}, _timerStateInterval{1000}
{
    _timerRead.setParent(this);
    _timerState.setParent(this);
    this->setDescription(dev.description);

    for (const auto &type : dev.readChannel)
        this->addReadSubChannel({type});

    for (const auto &type : dev.writeChannel)
        this->addWriteSubChannel({type});

    connect(&_timerRead, &QTimer::timeout, this, &Device::signalQueryRead, Qt::QueuedConnection);
    connect(&_timerState, &QTimer::timeout, this, &Device::signalQueryState, Qt::QueuedConnection);

    _timerRead.setInterval(_timerReadInterval);
    _timerState.setInterval(_timerStateInterval);

    _timerRead.start();
    _timerState.start();
}

void Device::update(const IOTV_SC::DEV_PKG &pkg)
{
    this->setId(pkg.id);
    this->setDescription(pkg.description);

    this->removeAllSubChannel();

    for (const auto &type : pkg.readChannel)
        this->addReadSubChannel({type});

    for (const auto &type : pkg.writeChannel)
        this->addWriteSubChannel({type});

    emit signalUpdate();
}

QString Device::getName() const
{
    return _name;
}

bool Device::isOnline() const
{
    return _state;
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

void Device::setState(bool newState)
{
    if (_state == newState)
        return;

    _state = newState;
    emit stateChanged();
}

bool operator==(const Device &lhs, const Device &rhs)
{
    if (std::make_tuple(lhs.getId(), lhs.getName(), lhs.getDescription(), lhs.getReadChannelLength(), lhs.getWriteChannelLength()) ==
            std::make_tuple(rhs.getId(), rhs.getName(), rhs.getDescription(), rhs.getReadChannelLength(), rhs.getWriteChannelLength()))
    {
        for (uint8_t i = 0; i < lhs.getReadChannelLength(); i++)
        {
            if (lhs.getReadChannelType(i) != rhs.getReadChannelType(i))
                return false;
        }
        return true;
    }
    return false;
}
