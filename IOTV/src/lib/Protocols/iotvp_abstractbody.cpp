#include "iotvp_abstractbody.h"

IOTVP_AbstractBody::IOTVP_AbstractBody() :
    _name("Non name"), _flags(static_cast<uint8_t>(FLAGS::NONE))
{

}

uint8_t IOTVP_AbstractBody::nameSize() const
{
    return _name.size();
}

uint8_t IOTVP_AbstractBody::flags() const
{
    return _flags;
}

const QString &IOTVP_AbstractBody::name() const
{
    return _name;
}

void IOTVP_AbstractBody::setName(const QString &newName)
{
    if (newName != _name)
        _name = newName;
}

void IOTVP_AbstractBody::setFlags(uint8_t newFlags)
{
    if (newFlags != _flags)
        _flags = newFlags;
}
