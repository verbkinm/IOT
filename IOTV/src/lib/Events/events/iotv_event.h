#pragma once

#include "base_host.h"

class IOTV_Event : public QObject
{
    Q_OBJECT

public:
    enum class EVENT_TYPE : uint8_t
    {
        NONE = 0,
        CONNECTING,
        DISCONNECTING,
        STATE,
        DATA,
        ALARM
    };

    IOTV_Event(EVENT_TYPE type, const Base_Host *host,
               QObject *parent = nullptr);

    const Base_Host *host() const;

    EVENT_TYPE type() const;

    bool isValid() const;

    friend bool operator==(const IOTV_Event &lhs, const IOTV_Event &rhs);
    friend bool operator<(const IOTV_Event &lhs, const IOTV_Event &rhs);

protected:
    const Base_Host *_host;

private:
    EVENT_TYPE _type;

signals:
    void signalEvent();
};
