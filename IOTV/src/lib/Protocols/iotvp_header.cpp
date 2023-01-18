#include "iotvp_header.h"

IOTVP_Header::IOTVP_Header() :
    _version(1), _type(TYPE::NONE), _appointment(APPOINTMRNT::NONE),
    _flags(FLAGS::NONE), _body(nullptr)
{

}

IOTVP_Header::~IOTVP_Header()
{

}

uint64_t IOTVP_Header::bodySize() const
{
    if (_body != nullptr)
        return _body->size() + _body->dataSize();

    return 0;
}

uint64_t IOTVP_Header::checkSum() const
{
    return version() + static_cast<uint8_t>(type()) + static_cast<uint8_t>(appointment()) + static_cast<uint8_t>(flags()) + bodySize();
}

uint64_t IOTVP_Header::size() const
{
    // Заголовок протокола (20 байт + N байт, N максимум 248) (документация)
    return 20 + bodySize();
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

IOTVP_Header::APPOINTMRNT IOTVP_Header::appointment() const
{
    return _appointment;
}

void IOTVP_Header::setAppointment(APPOINTMRNT newAppointment)
{
    if (_appointment != newAppointment)
        _appointment = newAppointment;
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

//IOTVP_AbstractBody *IOTVP_Header::body() const
//{
//    return _body;
//}

void IOTVP_Header::setBody(std::unique_ptr<IOTVP_AbstractBody> newBody)
{
    _body = std::move(newBody);
}

QByteArray IOTVP_Header::toData() const
{
    QByteArray result(size(), 0);
    result[0] = version();
    result[1] = static_cast<uint8_t>(type());
    result[2] = static_cast<uint8_t>(appointment());
    result[3] = static_cast<uint8_t>(flags());

    auto bSize = bodySize();
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
