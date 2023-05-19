#pragma once

#include "iotv_event.h"

class IOTV_Event_Connect : public IOTV_Event
{
public:
    IOTV_Event_Connect(const Base_Host *host, QObject *parent = nullptr);
};
