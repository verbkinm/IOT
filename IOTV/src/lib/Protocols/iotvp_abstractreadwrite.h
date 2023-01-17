#pragma once

#include "iotvp_abstractbody.h"

class IOTVP_AbstractReadWrite : public IOTVP_AbstractBody
{
public:
    virtual ~IOTVP_AbstractReadWrite() = default;

    virtual uint32_t dataSize() const override;
    const QByteArray &data() const;

    void setData(const QByteArray &newData);

private:
    QByteArray _data;
};

