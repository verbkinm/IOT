#pragma once

#include "channel.h"
#include "creatorpkgs.h"
#include "IOTV_SH.h"

class Base_Host : public QObject
{
public:
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

    State::State_STATE state() const;

    struct IOTV_Server_embedded *convert() const;

    static constexpr uint16_t TIMER_STATE_INTERVAL = 3000;
    static constexpr uint16_t TIMER_PING_INTERVAL = 1000;

protected:
    bool setReadChannelData(uint8_t channelNumber, const Raw &data);
    bool setReadChannelData(uint8_t channelNumber, const QByteArray &data);

    bool setWriteChannelData(uint8_t channelNumber, const Raw &data);

    void setReadChannel(const Channel &newReadChannel);
    void setWriteChannel(const Channel &newWriteChannel);

    void addWriteSubChannel(const Raw &data);
    void addReadSubChannel(const Raw &data);

    void removeAllSubChannel();

    void setId(uint16_t id);
    void setDescription(const QString description);

    State::State_STATE _state;

private:
    uint16_t _id;
    QString _description;

    Channel _readChannel;
    Channel _writeChannel;
};

