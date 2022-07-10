#pragma once

#include "channel.h"

class Write_Channel : public Channel
{
public:
    bool addSubchannel(Raw::DATA_TYPE dataType);

    virtual bool removeSubchannel(uint8_t index) override;
    virtual void removeAllSubchanel() override;
};


