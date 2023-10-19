#pragma once

#include "raw.h"
#include "log.h"

class Channel
{
public:
    void addSubchannel(const Raw &data);

    bool removeSubchannel(uint8_t channelNumber);
    void removeAllSubchanel();

    bool setData(uint8_t channelNumber, const Raw &data);
    bool setData(uint8_t channelNumber, const QByteArray &data);
    bool addData(uint8_t channelNumber, const QByteArray &data);
    void clearData(uint8_t channelNumber);

    const QByteArray &getData(uint8_t channelNumber) const;

    const Raw &getRawData(uint8_t channelNumber) const;
    Raw::DATA_TYPE getType(uint8_t channelNumber) const;

    uint8_t size() const;

private:
    std::vector<Raw> _data;
};
