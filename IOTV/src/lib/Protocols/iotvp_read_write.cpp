#include "iotvp_read_write.h"

IOTVP_READ_WRITE::IOTVP_READ_WRITE() :
    _channelNumber(0)
{
    _bodyType = BODY_TYPE::READ_WRITE;
}

uint8_t IOTVP_READ_WRITE::channelNumber() const
{
    return _channelNumber;
}

void IOTVP_READ_WRITE::setChannelNumber(uint8_t newChannelNumber)
{
    _channelNumber = newChannelNumber;
}

uint64_t IOTVP_READ_WRITE::size() const
{
    // Чтение / Запись данных (15 + N байт, N максимум 2^40) (документация)
    return 15 + nameSize() + dataSize();
}

uint64_t IOTVP_READ_WRITE::checkSum() const
{
    return nameSize() + channelNumber() + static_cast<uint8_t>(flags()) + dataSize();
}

QByteArray IOTVP_READ_WRITE::toData() const
{
    QByteArray result(size(), 0);
    result[0] = nameSize();
    result[1] = channelNumber();
    result[2] = static_cast<uint8_t>(flags());

    auto dSize = dataSize();
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        dSize = qToBigEndian(dSize);

    std::memcpy(&result[3], &dSize, 4); // Размер пакета данных = 4 байта (документация)

    auto chSum = checkSum();
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);

    std::memcpy(&result[7], &chSum, 8); // Контрольная сумма тела пакетах = 8 байта (документация)

    if (nameSize() > 0)
        std::memcpy(&result[7 + 8], name().toStdString().c_str(), nameSize());

    if (dataSize() > 0)
        std::memcpy(&result[7 + 8 + nameSize()], data().toStdString().c_str(), dataSize());

    return result;
}
