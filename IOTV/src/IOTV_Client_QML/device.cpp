#include "device.h"
#include "qdatetime.h"
#include <iostream>

Device::Device(const IOTV_Server_embedded *dev, QObject *parent)
    : Base_Host{static_cast<uint8_t>(dev->id), parent},
    _name{QByteArray{dev->name, dev->nameSize}},
    _aliasName(_name)
{
    Q_ASSERT(dev != nullptr);

    _timerRead.setParent(this);
    _timerState.setParent(this);
    this->setDescription(QByteArray{dev->description, dev->descriptionSize});

    for (int i = 0; i < dev->numberReadChannel; ++i)
        this->addReadSubChannel(Raw{static_cast<Raw::DATA_TYPE>(dev->readChannelType[i])});

    for (int i = 0; i < dev->numberWriteChannel; ++i)
        this->addWriteSubChannel(Raw{static_cast<Raw::DATA_TYPE>(dev->writeChannelType[i])});

//    connect(&_timerRead, &QTimer::timeout, this, &Device::slotTimerReadTimeOut);
//    connect(&_timerState, &QTimer::timeout, this, &Device::slotTimerStateTimeOut);

    _timerRead.setInterval(TIMER_READ_INTERVAL);
    _timerState.setInterval(TIMER_STATE_INTERVAL);

    _timerRead.start();
    _timerState.start();

    setState(true);
}

//!!!
void Device::update(const IOTV_Server_embedded *dev)
{
    Q_ASSERT(dev != nullptr);

    if (this->getId() == 0)
    {
        this->setId(dev->id);
        this->setDescription(QByteArray{dev->description, dev->descriptionSize});

        if (this->getReadChannelLength() != dev->numberReadChannel)
        {
            this->removeAllSubChannel();

            for (int i = 0; i < dev->numberReadChannel; ++i)
                this->addReadSubChannel(Raw{static_cast<Raw::DATA_TYPE>(dev->readChannelType[i])});

            for (int i = 0; i < dev->numberWriteChannel; ++i)
                this->addWriteSubChannel(Raw{static_cast<Raw::DATA_TYPE>(dev->writeChannelType[i])});
        }
        emit signalUpdate();
    }
}

QString Device::getName() const
{
    return _name;
}

bool Device::isOnline() const
{
    return state() == State_STATE_ONLINE;
}

bool Device::setData(uint8_t channelNumber, const QByteArray &data)
{
    if (data == getReadChannelData(channelNumber))
        return false;

    this->setReadChannelData(channelNumber, data);
    emit signalDataChanged(channelNumber);
    return true;
}

bool Device::addData(uint8_t channelNumber, const QByteArray &data)
{
    this->addReadChannelData(channelNumber, data);
    emit signalDataAdded(channelNumber);
    return true;
}

void Device::clearData(uint8_t channelNumber)
{
    clearReadChannelData(channelNumber);
}

void Device::setDataFromString(int channelNumber, QString data)
{
    if (data.isEmpty())
        return;

    emit signalQueryWrite(channelNumber, Raw::strToByteArray(data, getWriteChannelType(channelNumber)));
}

QString Device::readData(int channelNumber) const
{
    QByteArray data = getReadChannelData(channelNumber);
    Raw::DATA_TYPE type = getReadChannelType(channelNumber);
    return Raw::strData(data, type).first;
}

QString Device::readDataType(int channelNumber) const
{
    return Raw::strType(getReadChannelType(channelNumber));
}

QString Device::writeDataType(int channelNumber) const
{
    return Raw::strType(getWriteChannelType(channelNumber));
}

void Device::setLedColorManual(uint8_t ledNumder, bool red, bool green, bool blue)
{
    typedef enum {
        LED_RED,
        LED_GREEN,
        LED_BLUE,
        LED_YELLOW,
        LED_LIGTHBLUE,
        LED_MAGENTA,
        LED_WHITE,
        LED_NONE_COLOR
    } Led_RGB_color_t;

    Led_RGB_color_t color = LED_NONE_COLOR;

    if (red && !green && !blue)
        color = LED_RED;
    else if (!red && green && !blue)
        color = LED_GREEN;
    else if (!red && !green && blue)
        color = LED_BLUE;
    else if (red && green && !blue)
        color = LED_YELLOW;
    else if (!red && green && blue)
        color = LED_LIGTHBLUE;
    else if (red && !green && blue)
        color = LED_MAGENTA;
    else if (red && green && blue)
        color = LED_WHITE;

    uint8_t resultData = (ledNumder << 4) | color;

    QString data(QString::number(resultData, 16));

    qDebug() << data;

}

void Device::seriesAddData(QLineSeries *series, uint8_t channelNumber, uint8_t flags)
{
//    if (!_log_data_buff.contains(channelNumber))
//        return;

//    auto list = _log_data_buff[channelNumber];

//    series->clear();
//    QList<QPointF> points;

//    for (const auto &el : list)
//    {
//        if (el.flags == flags)
//        {
//            time_t seconds = el.timeMS / 1000;
//            std::tm *tm = localtime(&seconds);
//            float xVal = tm->tm_hour + tm->tm_min / 60 + tm->tm_sec / 3600;
//            float yVal = el.data.toFloat();
//            points.emplaceBack(xVal, yVal);
//        }
//    }
//    series->append(points);
//    _log_data_buff.erase(channelNumber);
}

void Device::setReadInterval(int interval)
{
    _timerRead.setInterval(interval);
}

void Device::setState(bool newState)
{
    Base_Host *host = this;

    //!!! Дублируются сигналы в Device и в Base_Host
    if (state() != static_cast<State_STATE>(newState))
    {
        host->setState(static_cast<State_STATE>(newState));
        emit signalStateChanged();
    }
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
    emit signalAliasNameChanged();
}

void Device::addLogData(const QByteArray &data, uint8_t channelNumber)
{
    _log_data_buff[channelNumber].append(data);
}

void Device::clearLogData(uint8_t channelNumber)
{
    _log_data_buff[channelNumber].clear();
}

QByteArray Device::logData(uint8_t channelNumber) const
{
    return _log_data_buff.at(channelNumber);
}

void Device::testFunc(Wrap_QByteArray *data)
{
    qDebug() << "testFunc total data: " << data->data().size();
    //    delete data;
}

void Device::slotTimerReadTimeOut()
{
    if (getId() == 0)
    {
        emit signalQueryIdentification();
        return;
    }

    emit signalQueryRead();
}

void Device::slotTimerStateTimeOut()
{
    if (getId() == 0)
    {
        emit signalQueryIdentification();
        return;
    }

    emit signalQueryState();
}
