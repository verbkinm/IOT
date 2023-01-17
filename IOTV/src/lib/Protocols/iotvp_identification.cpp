#include "iotvp_identification.h"

IOTVP_Identification::IOTVP_Identification() :
    _id(0), _description("None description")
{

}


uint16_t IOTVP_Identification::checkSum() const
{
    return id() + nameSize() + descriptionSize() + numberWriteChannel() + numberReadChannel() + flags();
}

uint16_t IOTVP_Identification::id() const
{
    return _id;
}

uint16_t IOTVP_Identification::descriptionSize() const
{
    return _description.size();
}

uint8_t IOTVP_Identification::numberWriteChannel() const
{
    return _writeChannel.size();
}

uint8_t IOTVP_Identification::numberReadChannel() const
{
    return _readChannel.size();
}

void IOTVP_Identification::setId(uint16_t newId)
{
    _id = newId;
}

const QString &IOTVP_Identification::description() const
{
    return _description;
}

void IOTVP_Identification::setDescription(const QString &newDescription)
{
    _description = newDescription;
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
    result[7] = flags();

    auto chSum = checkSum();
    std::memcpy(&result[8], &chSum, 16); // Контрольная сумма тела пакетах = 16 байта (документация)

    std::memcpy(&result[8 + 16], name().toStdString().c_str(), nameSize());
    std::memcpy(&result[8 + 16 + nameSize()], description().toStdString().c_str(), descriptionSize());
    std::memcpy(&result[8 + 16 + nameSize() + descriptionSize()], _writeChannel.data(), numberWriteChannel());
    std::memcpy(&result[8 + 16 + nameSize() + descriptionSize() + numberWriteChannel()], _readChannel.data(), numberReadChannel());

    return result;
}

uint64_t IOTVP_Identification::size() const
{
    // Идентификация устройства (24 + N байт, N максимум 2^40) (документация)
    return 24 + nameSize();
}
