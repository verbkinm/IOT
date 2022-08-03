#pragma once

#include "channel.h"

class Read_Channel : public Channel
{
public:
    bool addSubchannel(const Raw::DATA_TYPE dataType);
    ~Read_Channel();

    virtual bool removeSubchannel(uint8_t index) override;
    virtual void removeAllSubchanel() override;

    bool setData(uint8_t index, Raw rawData);

    Raw getData(uint8_t index) const;

    bool clearPointerRAW(uint8_t index);

private:
    std::vector<Raw> _data;
};
