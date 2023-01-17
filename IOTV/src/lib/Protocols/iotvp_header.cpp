#include "iotvp_header.h"

IOTVP_Header::IOTVP_Header() :
    _version(0), _type(TYPE::NONE), _appointment(APPOINTMRNT::NONE),
    _flags(FLAGS::NONE), _body(nullptr)
{

}

IOTVP_Header::~IOTVP_Header()
{
    if (_body != nullptr)
        delete _body;
}

uint64_t IOTVP_Header::bodySize() const
{
    if (_body != nullptr)
        return _body->dataSize();

    return 0;
}

uint16_t IOTVP_Header::checkSum() const
{
    return version() + static_cast<uint8_t>(type()) + static_cast<uint8_t>(appointment()) + static_cast<uint8_t>(flags()) + bodySize();
}

uint64_t IOTVP_Header::size() const
{
    // Заголовок протокола (28 байт + N байт, N максимум 248) (документация)
    return 28 + bodySize();
}

uint8_t IOTVP_Header::version() const
{
    return _version;
}

void IOTVP_Header::setVersion(uint8_t newVersion)
{
    _version = newVersion;
}

IOTVP_Header::TYPE IOTVP_Header::type() const
{
    return _type;
}

void IOTVP_Header::setType(TYPE newType)
{
    _type = newType;
}

IOTVP_Header::APPOINTMRNT IOTVP_Header::appointment() const
{
    return _appointment;
}

void IOTVP_Header::setAppointment(APPOINTMRNT newAppointment)
{
    _appointment = newAppointment;
}

IOTVP_Header::FLAGS IOTVP_Header::flags() const
{
    return _flags;
}

void IOTVP_Header::setFlags(FLAGS newFlags)
{
    _flags = newFlags;
}

IOTVP_AbstractBody *IOTVP_Header::body() const
{
    return _body;
}

void IOTVP_Header::setBody(IOTVP_AbstractBody *newBody)
{
    if (_body != nullptr)
        delete _body;

    _body = newBody;
}
