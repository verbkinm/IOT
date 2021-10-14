#ifndef BASE_HOST_H
#define BASE_HOST_H

#include <QTimer>

#include <set>
#include <map>

#include "read_channel.h"
#include "write_channel.h"

#define TIMER_INTERVAL 10000

class Base_Host : public QObject
{
    Q_OBJECT
public:
    Base_Host(QObject* parent = nullptr);
    virtual ~Base_Host();

    bool addReadSubChannel(Raw::DATA_TYPE dataType);
    bool addWriteSubChannel(Raw::DATA_TYPE dataType);
    void removeAllSubChannel();

    bool setReadChannelData(size_t channelNumber, Raw::RAW rawData);

    Raw::DATA_TYPE getReadChannelDataType(uint8_t channelNumber) const;
    Raw::RAW getReadChannelData(uint8_t channelNumber) const;
    Raw::DATA_TYPE getWriteChannelDataType(uint8_t channelNumber) const;

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

    void setId(uint8_t id);
    void setDescription(const std::string &description);

    uint8_t getId() const;
    std::string getDescription() const;

    virtual std::string getName() const = 0;
    virtual bool isConnected() const = 0;
    virtual int64_t readData(uint8_t channelNumber) = 0;
    virtual int64_t writeData(uint8_t channelNumber, Raw::RAW rawData) = 0;

protected:
    uint8_t _id;
    std::string _description;

    Read_Channel _readChannel;
    Write_Channel _writeChannel;

    std::set<uint8_t> _expectedResponseRead;
    std::map<uint8_t, Raw::RAW> _expectedResponseWrite;

    QTimer _timerResponse;

signals:
    void signalTimerResponse();
};

#endif // BASE_HOST_H
