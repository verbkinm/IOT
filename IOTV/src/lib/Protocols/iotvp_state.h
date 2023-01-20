#pragma once

#include "iotvp_abstractreadwrite.h"

class IOTVP_State : public IOTVP_AbstractReadWrite
{
public:
    IOTVP_State();

    enum class STATE : uint8_t
    {
        OFFLINE = 0,
        ONLINE
    };

    STATE state() const;
    virtual uint64_t checkSum() const override;
    virtual uint64_t size() const override;

    virtual QByteArray toData() const override;

    void setState(STATE newState);

    virtual bool operator==(const IOTVP_Abstract &obj) const override;

private:
    STATE _state;
};

