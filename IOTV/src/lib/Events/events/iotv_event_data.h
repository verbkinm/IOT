#pragma once

#include "iotv_event.h"
#include "raw.h"

class IOTV_Event_Data : public IOTV_Event
{   
public:
    enum class DATA_DIRECTION : uint8_t
    {
        NONE = 0,
        RX,
        TX
    };

    IOTV_Event_Data(const DATA_DIRECTION &direction, const Base_Host *host,
                    uint8_t channelNumber, const Raw &raw,
                    QObject *parent = nullptr);

    DATA_DIRECTION type() const;

private:
    DATA_DIRECTION _type;
    uint8_t _channelNumber;
    Raw _raw;

private slots:
    void slotCheckData(uint8_t channleNumber, Raw raw);
};

