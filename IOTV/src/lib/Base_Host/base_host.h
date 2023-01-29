#pragma once

#include "channel.h"
#include "creatorpkgs.h"
#include "IOTV_SH.h"

class Base_Host : public QObject
{
public:
    enum class STATE : uint8_t
    {
        OFFLINE = 0,
        ONLINE
    };

    Base_Host(uint16_t id = 0, QObject *parent = nullptr);
    virtual ~Base_Host() = default;

    uint16_t getId() const;
    QString getDescription() const;

    virtual QString getName() const = 0;

    uint8_t getReadChannelLength() const;
    uint8_t getWriteChannelLength() const;

    Raw::DATA_TYPE getReadChannelType(uint8_t channelNumber) const;
    Raw::DATA_TYPE getWriteChannelType(uint8_t channelNumber) const;

    QByteArray getReadChannelData(uint8_t channelNumber) const;

    STATE state() const;

    struct IOTV_Server_embedded *convert() const;

protected:
    bool setReadChannelData(uint8_t channelNumber, const Raw &data);
    bool setReadChannelData(uint8_t channelNumber, const QByteArray &data);

    bool setWriteChannelData(uint8_t channelNumber, const Raw &data);

    void setReadChannel(const Channel &newReadChannel);
    void setWriteChannel(const Channel &newWriteChannel);

    Raw getReadChannelRawData(uint8_t channelNumber) const;

    void addWriteSubChannel(const Raw &data);
    void addReadSubChannel(const Raw &data);

    void removeAllSubChannel();

    void setId(uint16_t id);
    void setDescription(const QString description);

    virtual bool isOnline() const = 0;

    void setState(STATE state);

private:
    uint16_t _id;
    QString _description;

    Channel _readChannel;
    Channel _writeChannel;

    STATE _state;

};

