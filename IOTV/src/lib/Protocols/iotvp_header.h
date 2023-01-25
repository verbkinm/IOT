#pragma once

#include <QByteArray>
#include <QtEndian>

#include "iotvp_abstractbody.h"

class IOTVP_Header : public IOTVP_Abstract
{
public:
    enum class TYPE : uint8_t
    {
        NONE = 0,
        REQUEST,
        RESPONSE
    };

    enum class ASSIGNMENT : uint8_t
    {
        NONE = 0,
        IDENTIFICATION,
        STATE,
        READ,
        WRITE,
        PING_PONG
    };

    IOTVP_Header();
    virtual ~IOTVP_Header() = default;

    uint8_t version() const;
    TYPE type() const;
    ASSIGNMENT assignment() const;
    FLAGS flags() const;

    virtual uint64_t dataSize() const override;
    virtual uint64_t checkSum() const override;

    virtual uint64_t size() const override;

    void setVersion(uint8_t newVersion);
    void setType(TYPE newType);
    void setAssignment(ASSIGNMENT newAppointment);
    void setFlags(FLAGS newFlags);
    void setBody(std::unique_ptr<IOTVP_AbstractBody> newBody);
    std::unique_ptr<IOTVP_AbstractBody> takeBody();

    virtual QByteArray toData() const override;

    bool operator==(const IOTVP_Abstract &obj) const override;

private:
    uint8_t _version;
    TYPE _type;
    ASSIGNMENT _assignment;
    FLAGS _flags;
    std::unique_ptr<IOTVP_AbstractBody> _body;
};


