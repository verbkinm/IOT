#pragma once

#include "iotvp_abstractreadwrite.h"

class IOTVP_STATE : public IOTVP_AbstractReadWrite
{
public:
    IOTVP_STATE();

    uint8_t state() const;
    virtual uint16_t checkSum() const override;
    virtual uint64_t size() const override;

    void setState(uint8_t newState);

    virtual QByteArray toData() const override;

private:
    uint8_t _state;
};

