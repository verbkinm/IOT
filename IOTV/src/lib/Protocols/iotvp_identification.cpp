#include "iotvp_identification.h"

IOTVP_Identification::IOTVP_Identification() :
    _id(0), _description("None description")
{
    _bodyType = BODY_TYPE::IDENTIFICATION;
}


uint64_t IOTVP_Identification::checkSum() const
{
    return id() + nameSize() + descriptionSize() + numberWriteChannel() + numberReadChannel() + static_cast<uint8_t>(flags());
}

uint16_t IOTVP_Identification::id() const
{
    return _id;
}

uint16_t IOTVP_Identification::descriptionSize() const
{
    return _description.toStdString().size();
}

uint8_t IOTVP_Identification::numberWriteChannel() const
{
    return (_writeChannel.size() > std::numeric_limits<uint8_t>::max()) ? std::numeric_limits<uint8_t>::max() : _writeChannel.size();
}

uint8_t IOTVP_Identification::numberReadChannel() const
{
    return (_readChannel.size() > std::numeric_limits<uint8_t>::max()) ? std::numeric_limits<uint8_t>::max() : _readChannel.size();
}

void IOTVP_Identification::setId(uint16_t newId)
{
    if (_id != newId)
        _id = newId;
}

const QString &IOTVP_Identification::description() const
{
    return _description;
}

void IOTVP_Identification::setDescription(const QString &newDescription)
{
    if (_description != newDescription)
        _description = (newDescription.toStdString().size() > std::numeric_limits<uint16_t>::max()) ? newDescription.mid(0, std::numeric_limits<uint16_t>::max()) : newDescription;
}

QByteArray IOTVP_Identification::toData() const
{
    QByteArray result(size(), 0);
    result[0] = (id() >> 8);
    result[1] = id();

    result[2] = nameSize();

    result[3] = (descriptionSize() >> 8);
    result[4] = descriptionSize();

    result[5] = numberWriteChannel();
    result[6] = numberReadChannel();
    result[7] = static_cast<uint8_t>(flags());

    auto chSum = checkSum();
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);

    std::memcpy(&result[8], &chSum, 8); // Контрольная сумма тела пакетах = 8 байта (документация)

    if (nameSize() > 0)
        std::memcpy(&result[8 + 8], name().toStdString().c_str(), nameSize());

    if (descriptionSize() > 0)
        std::memcpy(&result[8 + 8 + nameSize()], description().toStdString().c_str(), descriptionSize());

    if (numberWriteChannel() > 0)
        std::memcpy(&result[8 + 8 + nameSize() + descriptionSize()], _writeChannel.data(), numberWriteChannel());

    if (numberReadChannel() > 0)
        std::memcpy(&result[8 + 8 + nameSize() + descriptionSize() + numberWriteChannel()], _readChannel.data(), numberReadChannel());

    return result;
}

uint64_t IOTVP_Identification::size() const
{
    // Идентификация устройства (16 + N байт, N максимум 2^40) (документация)
    return 16 + nameSize() + descriptionSize() + numberWriteChannel() + numberReadChannel();
}


uint32_t IOTVP_Identification::dataSize() const
{
    return 0;
}

const QList<Raw::DATA_TYPE> &IOTVP_Identification::readChannel() const
{
    return _readChannel;
}

void IOTVP_Identification::setReadChannel(const QList<Raw::DATA_TYPE> &newReadChannel)
{
    if (_readChannel != newReadChannel)
        _readChannel = (newReadChannel.size() > std::numeric_limits<uint8_t>::max()) ? newReadChannel.mid(0, std::numeric_limits<uint8_t>::max()): newReadChannel;
}

const QList<Raw::DATA_TYPE> &IOTVP_Identification::writeChannel() const
{
    return _writeChannel;
}

void IOTVP_Identification::setWriteChannel(const QList<Raw::DATA_TYPE> &newWriteChannel)
{
    if (_writeChannel != newWriteChannel)
        _writeChannel = (newWriteChannel.size() > std::numeric_limits<uint8_t>::max()) ? newWriteChannel.mid(0, std::numeric_limits<uint8_t>::max()) : newWriteChannel;
}
