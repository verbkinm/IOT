#include "iotvp_abstractbody.h"

IOTVP_AbstractBody::IOTVP_AbstractBody(BODY_TYPE type) : _bodyType(type),
    _name("Non name"), _flags(FLAGS::NONE)
{

}

uint8_t IOTVP_AbstractBody::nameSize() const
{
    return (_name.toStdString().size() > std::numeric_limits<uint8_t>::max()) ? std::numeric_limits<uint8_t>::max() : _name.toStdString().size();
}

IOTVP_AbstractBody::FLAGS IOTVP_AbstractBody::flags() const
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
        _name = (newName.toLocal8Bit().size() > std::numeric_limits<uint8_t>::max()) ? newName.toLocal8Bit().mid(0, std::numeric_limits<uint8_t>::max()) : newName;
}

void IOTVP_AbstractBody::setFlags(IOTVP_AbstractBody::FLAGS newFlags)
{
    if (newFlags != _flags)
        _flags = newFlags;
}

//void IOTVP_AbstractBody::setBodyType(BODY_TYPE newBodyType)
//{
//    if (_bodyType != newBodyType)
//        _bodyType = newBodyType;
//}

IOTVP_AbstractBody::BODY_TYPE IOTVP_AbstractBody::bodyType() const
{
    return _bodyType;
}
