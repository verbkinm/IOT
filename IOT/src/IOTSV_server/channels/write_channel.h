#ifndef WRITE_CHANNEL_H
#define WRITE_CHANNEL_H

#include "channel.h"

class Write_Channel : public Channel
{
public:
    bool addSubchannel(Raw::DATA_TYPE dataType);

    virtual bool removeSubchannel(uint8_t index) override;
    virtual void removeAllSubchanel() override;

    bool setRange(uint8_t index);
};

#endif // WRITE_CHANNEL_H
