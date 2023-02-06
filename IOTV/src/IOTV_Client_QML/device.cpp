#include "device.h"

Device::Device(const IOTV_Server_embedded *dev, QObject *parent)
    : Base_Host{static_cast<uint8_t>(dev->id), parent}, _name{QByteArray{dev->name, dev->nameSize}},
      _timerReadInterval{1000}, _timerStateInterval{1000}
{
    Q_ASSERT(dev != nullptr);

    _timerRead.setParent(this);
    _timerState.setParent(this);
    this->setDescription(QByteArray{dev->description, dev->descriptionSize});

    for (int i = 0; i < dev->numberReadChannel; ++i)
        this->addReadSubChannel(static_cast<Raw::DATA_TYPE>(dev->readChannelType[i]));

    for (int i = 0; i < dev->numberWriteChannel; ++i)
        this->addWriteSubChannel(static_cast<Raw::DATA_TYPE>(dev->writeChannelType[i]));

    connect(&_timerRead, &QTimer::timeout, this, &Device::signalQueryRead, Qt::QueuedConnection);
    connect(&_timerState, &QTimer::timeout, this, &Device::signalQueryState, Qt::QueuedConnection);

    _timerRead.setInterval(_timerReadInterval);
    _timerState.setInterval(_timerStateInterval);

    _timerRead.start();
    _timerState.start();
}

void Device::update(const IOTV_Server_embedded *dev)
{
    Q_ASSERT(dev != nullptr);

    this->setId(dev->id);
    this->setDescription(QByteArray{dev->description, dev->descriptionSize});

    this->removeAllSubChannel();

    for (int i = 0; i < dev->numberReadChannel; ++i)
        this->addReadSubChannel(static_cast<Raw::DATA_TYPE>(dev->readChannelType[i]));

    for (int i = 0; i < dev->numberWriteChannel; ++i)
        this->addWriteSubChannel(static_cast<Raw::DATA_TYPE>(dev->writeChannelType[i]));

    emit signalUpdate();
}

QString Device::getName() const
{
    return _name;
}

bool Device::isOnline() const
{
    if (_state == State::State_STATE_ONLINE)
        return true;

    return false;
}

bool Device::setData(uint8_t channelNumber, const QByteArray &data)
{
    return this->setReadChannelData(channelNumber, data);
}

void Device::setDataFromString(int channelNumber, QString data)
{
    if (data.isEmpty())
        return;

    emit signalQueryWrite(channelNumber, Raw::strToByteArray(data, getWriteChannelType(channelNumber)));
}

QString Device::readData(int channelNumber) const
{
    return Raw::strData(getReadChannelData(channelNumber), getReadChannelType(channelNumber)).first;
}

QString Device::readDataType(int channelNumber) const
{
    return Raw::strData(getReadChannelData(channelNumber), getReadChannelType(channelNumber)).second;
}

QString Device::writeDataType(int channelNumber) const
{
    return Raw::strData(QByteArray {}, getWriteChannelType(channelNumber)).second;
}

void Device::setReadInterval(int interval)
{
    _timerRead.setInterval(interval);
}

void Device::setState(bool newState)
{
    if (getId() == 0)
    {
        emit signalIdentification();
//        return;
    }

    if (_state == static_cast<State::State_STATE>(newState))
        return;

    _state = static_cast<State::State_STATE>(newState);
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

bool operator!=(const Device &lhs, const Device &rhs)
{
    return !(lhs == rhs);
}

const QString &Device::aliasName() const
{
    return _aliasName;
}

void Device::setAliasName(const QString &newAliasName)
{
    if (_aliasName == newAliasName)
        return;
    _aliasName = newAliasName;
    emit aliasNameChanged();
}
