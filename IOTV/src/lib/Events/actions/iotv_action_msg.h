#pragma once

#include "iotv_action.h"
#include "raw.h"
#include "base_host.h"
#include "log.h"

class IOTV_Action_MSG : public IOTV_Action
{
public:
    explicit IOTV_Action_MSG(const QString &msg, QObject *parent = nullptr);
    virtual ~IOTV_Action_MSG() = default;

    virtual void exec() override;
    virtual bool isValid() const override;

private:
    QString _msg;
};

