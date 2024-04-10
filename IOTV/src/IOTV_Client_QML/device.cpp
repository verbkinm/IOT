#include "device.h"

#include <QBuffer>
#include <QDateTime>
#include <QFile>

#include <sstream>

Device::Device(const IOTV_Server_embedded *dev, QObject *parent)
    : Base_Host{static_cast<uint8_t>(dev->id), parent},
    _name{QByteArray{dev->name, dev->nameSize}},
    _aliasName(_name),
    _logDataOverflow(false)
{
    Q_ASSERT(dev != nullptr);

    _timerRead.setParent(this);
    _timerState.setParent(this);
    this->setDescription(QByteArray{dev->description, dev->descriptionSize});

    for (int i = 0; i < dev->numberReadChannel; ++i)
        this->addReadSubChannel(Raw{static_cast<Raw::DATA_TYPE>(dev->readChannelType[i])});

    for (int i = 0; i < dev->numberWriteChannel; ++i)
        this->addWriteSubChannel(Raw{static_cast<Raw::DATA_TYPE>(dev->writeChannelType[i])});

    connect(&_timerRead, &QTimer::timeout, this, &Device::slotTimerReadTimeOut);
    connect(&_timerState, &QTimer::timeout, this, &Device::slotTimerStateTimeOut);

    _timerRead.setInterval(TIMER_READ_INTERVAL);
    _timerState.setInterval(TIMER_STATE_INTERVAL);

    _timerRead.start();
    _timerState.start();
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
    emit signalDataChanged(channelNumber, data);
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

float convert_range(float value, float From1, float From2, float To1, float To2)
{
    return (value-From1)/(From2-From1)*(To2-To1)+To1;
}

void Device::dataLogToPoints(uint8_t channelNumber, uint8_t flags)
{
//    auto start = std::chrono::system_clock::now();

    if (!_log_data_buf.contains(channelNumber) || _log_data_buf[channelNumber].size() == 0)
    {
        emit signalResponceLogData({}, channelNumber, flags);
        return;
    }

    QBuffer bufferFile(&_log_data_buf[channelNumber]);
    bufferFile.open(QIODevice::ReadOnly);

    //    QFile fileTmp("test_" + QString::number(channelNumber) + ".txt");
    //    fileTmp.open(QIODevice::WriteOnly);
    //    fileTmp.write(_log_data_buf[channelNumber].data(), _log_data_buf[channelNumber].size());
    //    fileTmp.close();

    std::map<uint64_t, float> uniqPoints;
    while (!bufferFile.atEnd())
    {
        std::string line = bufferFile.readLine().toStdString();
        std::stringstream stream(line);

        float yVal = 0;
        //        float xVal = 0;
        uint64_t ms = 0;
        std::string dataStr;
        QString data;

        stream >> ms >> dataStr;
        data.append(dataStr);

        if (stream.fail())
        {
//            qDebug() << Q_FUNC_INFO << "ошибка данных" << channelNumber;
                continue;
        }

        Raw raw = getReadChannelDataRaw(channelNumber);
        if (raw.isInt() || raw.isReal())
            yVal = data.toFloat();
        else if (raw.isBool())
            yVal = data == "true" ? 1 : 0;

        uniqPoints[ms] = yVal;
    }
    bufferFile.close();
    _log_data_buf.erase(channelNumber);

    QList<QPointF> points;
    points.emplace_back(uniqPoints.begin()->first, uniqPoints.begin()->second);
    for (auto it = ++uniqPoints.begin(); it != uniqPoints.end(); it++)
    {
        auto prev = std::prev(it);

        if (it->second != prev->second)
        {
            points.emplace_back(prev->first, prev->second);

            if (it->second == std::next(it)->second)
                points.emplace_back(it->first, it->second);
        }
    }
    points.emplace_back(uniqPoints.rbegin()->first, uniqPoints.rbegin()->second);

        qDebug() << "Количество точек list" << points.size();
        qDebug() << "Количество точек map" << uniqPoints.size();

    uniqPoints.clear();
    //    for (auto [key, value] : uniqPoints)
    //        points.append({key / roundOffset, value}); // не забываем вернуть запятую на место!

//    qDebug() << "Количество точек" << points.size();
    emit signalResponceLogData(std::move(points), channelNumber, flags);

//    qDebug() << "dataLogToPoints, channel - " << channelNumber << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
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

void Device::addDataLog(uint8_t channelNumber, const QByteArray &data)
{
    if ((_log_data_buf[channelNumber].size() + data.size()) <= LOG_DATA_MAX)
    {
        _log_data_buf[channelNumber].append(data);
        _logDataOverflow = false;
    }
    else
        _logDataOverflow = true;
}

void Device::clearDataLog(uint8_t channelNumber)
{
    _log_data_buf.erase(channelNumber);
}

bool Device::logDataOverflow() const
{
    return _logDataOverflow;
}

void Device::testFunc(Wrap_QByteArray *data)
{
    qDebug() << "testFunc total data: " << data->data().size();
    //    delete data;
}

int Device::getLOG_DATA_MAX()
{
    return LOG_DATA_MAX;
}

void Device::fillSeries(QLineSeries *series, QList<QPointF> points)
{
    // намного быстрее добавляются точки, чем через qml series.append(x, y)
    series->replace(points);
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
