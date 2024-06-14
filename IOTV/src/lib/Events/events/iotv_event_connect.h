#pragma once

#include "iotv_event.h"

class IOTV_Event_Connect : public IOTV_Event
{
public:
    explicit IOTV_Event_Connect(const Base_Host *host, QObject *parent = nullptr);

private:
    virtual void runActions() override;
    virtual bool isValid() const override;
};
