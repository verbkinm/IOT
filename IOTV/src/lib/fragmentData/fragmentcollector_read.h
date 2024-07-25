#pragma once

#include "fragmentcollector.h"

class Fragment_Collector_Read  : public Fragment_Collector
{
public:
    explicit Fragment_Collector_Read(size_t max_buff_size);
    virtual RAII_Header pkg() override;
};

