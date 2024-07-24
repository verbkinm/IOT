#pragma once

#include "fragmentcollector.h"

class FragmentCollector_Identification : public Fragment_Collector
{
public:
    explicit FragmentCollector_Identification(size_t max_buff_size);
    virtual RAII_Header pkg() override;
};

