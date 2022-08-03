#pragma once

#include "raw.h"
#include "log.h"

class Channel
{
public:
    virtual bool removeSubchannel(uint8_t channelNumber) = 0;
    virtual void removeAllSubchanel() = 0;

    bool setDataType(uint8_t index, Raw::DATA_TYPE dataType);

//    Raw::DATA_TYPE getDataType(uint8_t channelNumber) const;

    uint8_t length() const;

protected:
    не нужен отдельный вектор для данных и типа
//    std::vector<Raw::DATA_TYPE> _dataType;
};
