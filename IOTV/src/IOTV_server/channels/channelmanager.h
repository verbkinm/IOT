#ifndef CHANNELMANAGER_H
#define CHANNELMANAGER_H

#include <QTimer>

#include <set>
#include <map>

#include "channels/read_channel.h"
#include "channels/write_channel.h"

#define TIMER_INTERVAL 10000

class ChannelManager : public QObject
{
    Q_OBJECT
public:
    ChannelManager(QObject* parent = nullptr);

    bool addReadSubChannel(Raw::DATA_TYPE dataType);
    bool addWriteSubChannel(Raw::DATA_TYPE dataType);
    void removeAllSubChannel();

    bool setReadChannelData(size_t channelNumber, Raw::RAW rawData);

    Raw::DATA_TYPE getReadChannelDataType(size_t index) const;
    Raw::RAW getReadChannelData(size_t index) const;
    Raw::DATA_TYPE getWriteChannelDataType(size_t index) const;

    const std::set<uint8_t> &getExpectedResponseRead() const;
    const std::map<uint8_t, Raw::RAW> &getExpectedResponseWrite() const;

    size_t readChannelLength() const;
    size_t writeChannelLength() const;

    bool insertExpectedResponseRead(uint8_t channelNumber);
    bool insertExpectedResponseWrite(uint8_t channelNumber, Raw::RAW rawData);

    void eraseExpectedResponseRead(uint8_t channelNumber);
    void eraseExpectedResponseWrite(uint8_t channelNumber);
    void eraseAllExpectedResponse();

    void stopTimer();

private:
    Read_Channel _readChannel;
    Write_Channel _writeChannel;

    std::set<uint8_t> _expectedResponseRead;
    std::map<uint8_t, Raw::RAW> _expectedResponseWrite;

    QTimer _timerResponse;

signals:
    void signalTimerResponse();
};

#endif // CHANNELMANAGER_H
