#include "iotvp_header.h"
#include <cstring>

IOTVP_Header::IOTVP_Header() :
    _version(0), _type(TYPE::NONE), _assignment(ASSIGNMENT::NONE),
    _flags(FLAGS::NONE)
{

}

uint64_t IOTVP_Header::dataSize() const
{
    if (_body != nullptr)
        return _body->size();

    return 0;
}

uint64_t IOTVP_Header::checkSum() const
{
    return version() + static_cast<uint8_t>(type()) + static_cast<uint8_t>(assignment()) + static_cast<uint8_t>(flags()) + dataSize();
}

uint64_t IOTVP_Header::size() const
{  
    return IOTVP_Abstract::HEADER_SIZE + dataSize();
}

uint8_t IOTVP_Header::version() const
{
    return _version;
}

void IOTVP_Header::setVersion(uint8_t newVersion)
{
    if (_version != newVersion)
        _version = newVersion;
}

IOTVP_Header::TYPE IOTVP_Header::type() const
{
    return _type;
}

void IOTVP_Header::setType(TYPE newType)
{
    if (_type != newType)
        _type = newType;
}

IOTVP_Header::ASSIGNMENT IOTVP_Header::assignment() const
{
    return _assignment;
}

void IOTVP_Header::setAssignment(ASSIGNMENT newAppointment)
{
    if (_assignment != newAppointment)
        _assignment = newAppointment;
}

IOTVP_Header::FLAGS IOTVP_Header::flags() const
{
    return _flags;
}

void IOTVP_Header::setFlags(FLAGS newFlags)
{
    if (_flags != newFlags)
        _flags = newFlags;
}

void IOTVP_Header::setBody(std::unique_ptr<IOTVP_AbstractBody> newBody)
{
    _body = std::move(newBody);
}

std::unique_ptr<IOTVP_AbstractBody> IOTVP_Header::takeBody()
{
    return std::move(_body);
}

QByteArray IOTVP_Header::toData() const
{
    QByteArray result(size() - dataSize(), 0);
    result[0] = version();
    result[1] = static_cast<uint8_t>(type());
    result[2] = static_cast<uint8_t>(assignment());
    result[3] = static_cast<uint8_t>(flags());

    auto bSize = dataSize();
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        bSize = qToBigEndian(bSize);

    std::memcpy(&result[4], &bSize, 8); // Размер тела протокола = 8 байта (документация)

    auto chSum = checkSum();
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);

    std::memcpy(&result[12], &chSum, 8); // Контрольная сумма тела пакетах = 8 байта (документация)

    if (_body != nullptr)
        result.append(_body->toData());

    return result;
}

bool IOTVP_Header::operator==(const IOTVP_Abstract &obj) const
{
    const IOTVP_Header *ptr = dynamic_cast<const IOTVP_Header*>(&obj);
    if (ptr == nullptr)
        return false;

    return  ( (version() == ptr->version())
              && (type() == ptr->type())
              && (assignment() == ptr->assignment())
              && (flags() == ptr->flags())
              && (_body.get() == ptr->_body.get()) );
}

