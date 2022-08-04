#pragma once

#include "channel.h"
#include "IOTV_SH.h"
#include "Base_Host_global.h"

class BASE_HOST_EXPORT Base_Host
{
public:
    Base_Host(uint8_t id = 0);
    virtual ~Base_Host() = default;

protected:
    bool setReadChannelData(uint8_t channelNumber, const Raw &data);
    bool setReadChannelData(uint8_t channelNumber, const std::vector<uint8_t> &data);

    bool setWriteChannelData(uint8_t channelNumber, const Raw &data);

    void setReadChannel(const Channel &newReadChannel);
    void setWriteChannel(const Channel &newWriteChannel);

    void addWriteSubChannel(const Raw &data);
    void addReadSubChannel(const Raw &data);

    void removeAllSubChannel();

    void setId(uint8_t id);
    void setDescription(const std::string &description);

    uint8_t getId() const;
    std::string getDescription() const;

    Raw::DATA_TYPE getReadChannelType(uint8_t channelNumber) const;
    Raw::DATA_TYPE getWriteChannelType(uint8_t channelNumber) const;

    Raw getReadChannelData(uint8_t channelNumber) const;

    uint8_t readChannelLength() const;
    uint8_t writeChannelLength() const;

    virtual std::string getName() const = 0;

    virtual bool isOnline() const = 0;
    virtual void setOnline(bool state) = 0;

    virtual qint64 readData(uint8_t channelNumber) = 0;
    virtual qint64 writeData(uint8_t channelNumber, const Raw &rawData) = 0;
    virtual void dataResived(const IOTV_SH::RESPONSE_PKG &data) = 0;

    virtual qint64 writeToServer(const QByteArray &data) = 0;

    uint8_t _id;
    std::string _description;

    Channel _readChannel;
    Channel _writeChannel;
};

