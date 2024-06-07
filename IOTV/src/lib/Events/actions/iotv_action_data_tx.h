#pragma once

#include "iotv_action.h"
#include "base_host.h"

class IOTV_Action_Data_TX : public IOTV_Action
{
public:
    IOTV_Action_Data_TX(Base_Host *host, uint8_t channelNumber, const QString &data, QObject *parent = nullptr);
    virtual ~IOTV_Action_Data_TX() = default;

    const Base_Host *host() const;

    uint8_t channelNumber() const;

    const QString &data() const;

    virtual void exec() override;
    virtual bool isValid() const override;

    QString hostName() const;
    void setHostName(const QString &newHostName);

private:
    Base_Host *_host;
    QString _hostName;
    uint8_t _channelNumber;
    QString _data;
};

