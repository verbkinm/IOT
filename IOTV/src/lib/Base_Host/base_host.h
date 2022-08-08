#pragma once

#include "channel.h"
#include "IOTV_SH.h"
#include "Base_Host_global.h"

class BASE_HOST_EXPORT Base_Host : public QObject
{
public:
    Base_Host(uint8_t id = 0, QObject *parent = nullptr);
    virtual ~Base_Host() = default;

protected:
    bool setReadChannelData(uint8_t channelNumber, const Raw &data);
    bool setReadChannelData(uint8_t channelNumber, const QByteArray &data);

    bool setWriteChannelData(uint8_t channelNumber, const Raw &data);

    void setReadChannel(const Channel &newReadChannel);
    void setWriteChannel(const Channel &newWriteChannel);

    void addWriteSubChannel(const Raw &data);
    void addReadSubChannel(const Raw &data);

    void removeAllSubChannel();

    void setId(uint8_t id);
    void setDescription(const QString description);

    uint8_t getId() const;
    QString getDescription() const;

    Raw::DATA_TYPE getReadChannelType(uint8_t channelNumber) const;
    Raw::DATA_TYPE getWriteChannelType(uint8_t channelNumber) const;

    Raw getReadChannelData(uint8_t channelNumber) const;

    uint8_t getReadChannelLength() const;
    uint8_t getWriteChannelLength() const;

    virtual QString getName() const = 0;

    virtual bool isOnline() const = 0;

    virtual qint64 readData(uint8_t channelNumber) = 0;
    virtual qint64 writeData(uint8_t channelNumber, const QByteArray &rawData) = 0;
    virtual void dataResived(QByteArray data) = 0;

    virtual qint64 writeToServer(const QByteArray &data) = 0;

    uint8_t _id;
    QString _description;

    Channel _readChannel;
    Channel _writeChannel;
};

