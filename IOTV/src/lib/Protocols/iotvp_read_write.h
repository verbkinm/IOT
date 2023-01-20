#pragma once

#include "iotvp_abstractreadwrite.h"

class IOTVP_READ_WRITE : public IOTVP_AbstractReadWrite
{
public:
    IOTVP_READ_WRITE();
    virtual ~IOTVP_READ_WRITE() = default;

    uint8_t channelNumber() const;
    virtual uint64_t checkSum() const override;
    virtual uint64_t size() const override;

    virtual QByteArray toData() const override;

    void setChannelNumber(uint8_t newChannelNumber);

    bool operator==(const IOTVP_Abstract &obj) const override;

private:
    uint8_t _channelNumber;
};
