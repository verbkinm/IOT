#pragma once

#include "iotv_event.h"

class IOTV_Event_State : public IOTV_Event
{   
public:
    enum class STATE_TYPE : uint8_t
    {
        NONE = 0,
        ONLINE,
        OFFLINE,
        SWITCH
    };

    IOTV_Event_State(STATE_TYPE state, const Base_Host *host,
                     QObject *parent = nullptr);

    STATE_TYPE state() const;

private:
    STATE_TYPE _state;
};

