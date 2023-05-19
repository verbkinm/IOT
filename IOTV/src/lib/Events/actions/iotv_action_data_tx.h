#pragma once

#include "iotv_action.h"
#include "raw.h"
#include "base_host.h"

class IOTV_Action_Data_TX : public IOTV_Action
{
public:
    IOTV_Action_Data_TX(Base_Host *host, uint8_t channelNumber, const QByteArray &data, QObject *parent = nullptr);
    virtual ~IOTV_Action_Data_TX() = default;

    const Base_Host *host() const;

    uint8_t channelNumber() const;

    const QByteArray &data() const;

    virtual void exec() override;
    virtual bool isValid() const override;

private:
    Base_Host *_host;
    uint8_t _channelNumber;
    QByteArray _data;
};

