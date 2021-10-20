#ifndef BASE_HOST_H
#define BASE_HOST_H

#include <QTimer>

#include <set>
#include <map>

#include "read_channel.h"
#include "write_channel.h"
#include "Base_Host_global.h"

#define TIMER_INTERVAL 5000

class BASE_HOST_EXPORT Base_Host : public QObject
{
    Q_OBJECT
public:
    Base_Host(uint8_t id = 0, QObject* parent = nullptr);
    virtual ~Base_Host();

    bool addReadSubChannel(Raw::DATA_TYPE dataType);
    bool addWriteSubChannel(Raw::DATA_TYPE dataType);
    void removeAllSubChannel();

    void setId(uint8_t id);
    void setDescription(const QString &description);
    bool setReadChannelData(size_t channelNumber, Raw::RAW rawData);

    uint8_t getId() const;
    QString getDescription() const;
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

    virtual QString getName() const = 0;
    virtual bool getState() const = 0;
    virtual void setState(bool state) = 0;
    virtual qint64 readData(uint8_t channelNumber) = 0;
    virtual qint64 writeData(uint8_t channelNumber, Raw::RAW &rawData) = 0;
    virtual void dataResived(QByteArray data) = 0;

    virtual qint64 writeToServer(QByteArray &data) = 0;

protected:
    uint8_t _id;
    QString _description;

    Read_Channel _readChannel;
    Write_Channel _writeChannel;

    std::set<uint8_t> _expectedResponseRead;
    std::map<uint8_t, Raw::RAW> _expectedResponseWrite;

    QTimer _timerResponseRead;
    QTimer _timerResponseWrite;
};

#endif // BASE_HOST_H
