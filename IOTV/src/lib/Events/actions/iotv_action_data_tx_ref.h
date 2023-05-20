#pragma once

#include "iotv_action.h"
#include "raw.h"
#include "base_host.h"

class IOTV_Action_Data_TX_Ref : public IOTV_Action
{
public:
    IOTV_Action_Data_TX_Ref(Base_Host *dstHost, uint8_t dstChannelNumber,
                            Base_Host *srcHost, uint8_t srcChannelNumber,
                            QObject *parent = nullptr);
    virtual ~IOTV_Action_Data_TX_Ref() = default;

    virtual void exec() override;
    virtual bool isValid() const override;

    const Base_Host *dstHost() const;
    const Base_Host *srcHost() const;

    uint8_t dstChannelNumber() const;
    uint8_t srcChannelNumber() const;

private:
    Base_Host *_dstHost, *_srcHost;
    uint8_t _dstChannelNumber, _srcChannelNumber;
};

