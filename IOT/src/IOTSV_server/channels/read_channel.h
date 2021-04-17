#ifndef READ_CHANNEL_H
#define READ_CHANNEL_H

#include "channel.h"

class Read_Channel : public Channel
{
public:
    bool addSubchannel(const Raw::DATA_TYPE dataType);

    virtual bool removeSubchannel(uint8_t index) override;
    virtual void removeAllSubchanel() override;

    bool setData(uint8_t index, Raw::RAW rawData);

    Raw::RAW getData(uint8_t index) const;


private:
    std::vector<Raw::RAW> _data;
};

#endif // READ_CHANNEL_H
