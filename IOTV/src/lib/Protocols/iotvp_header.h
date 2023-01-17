#pragma once

#include "iotvp_abstractbody.h"

class IOTVP_Header
{
public:
    enum class TYPE : uint8_t
    {
        NONE = 0,
        REQUEST,
        RESPONSE
    };

    enum class APPOINTMRNT : uint8_t
    {
        NONE = 0,
        IDENTIFICATION,
        STATE,
        READ,
        WRITE,
        PING_PONG
    };

    enum class FLAGS : uint8_t
    {
        NONE = 0,
        ERROR = 0xFF
    };

    IOTVP_Header();
    ~IOTVP_Header();

    uint8_t version() const;
    TYPE type() const;
    APPOINTMRNT appointment() const;
    FLAGS flags() const;
    uint64_t bodySize() const;
    uint16_t checkSum() const;

    IOTVP_AbstractBody *body() const;
    uint64_t size() const;

    void setVersion(uint8_t newVersion);
    void setType(TYPE newType);
    void setAppointment(APPOINTMRNT newAppointment);
    void setFlags(FLAGS newFlags);

    void setBody(IOTVP_AbstractBody *newBody);

private:
    uint8_t _version;
    TYPE _type;
    APPOINTMRNT _appointment;
    FLAGS _flags;
    uint64_t _size;
    IOTVP_AbstractBody *_body;
};

