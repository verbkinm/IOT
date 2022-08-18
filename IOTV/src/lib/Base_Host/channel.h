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

    QByteArray getData(uint8_t channelNumber) const;
    Raw::DATA_TYPE getType(uint8_t channelNumber) const;

    uint8_t size() const;

private:
    std::vector<Raw> _data;
};
