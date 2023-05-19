#pragma once

#include "iotv_event.h"

class IOTV_Event_Disconnect : public IOTV_Event
{
public:
    IOTV_Event_Disconnect(const Base_Host *host, QObject *parent = nullptr);
};
