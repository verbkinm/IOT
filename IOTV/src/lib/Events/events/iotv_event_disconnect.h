#pragma once

#include "iotv_event.h"

class IOTV_Event_Disconnect : public IOTV_Event
{
public:
    explicit IOTV_Event_Disconnect(const Base_Host *host, QObject *parent = nullptr);

private:
    virtual void runActions() override;
    virtual bool isValid() const override;
};
