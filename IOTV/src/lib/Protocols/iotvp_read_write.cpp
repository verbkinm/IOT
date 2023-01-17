#include "iotvp_read_write.h"

IOTVP_READ_WRITE::IOTVP_READ_WRITE() :
    _channelNumber(0)
{

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
    // Чтение / Запись данных (23 + N байт, N максимум 2^40) (документация)
    return 23 + nameSize() + dataSize();
}

uint16_t IOTVP_READ_WRITE::checkSum() const
{
    return nameSize() + channelNumber() + flags() + dataSize();
}

QByteArray IOTVP_READ_WRITE::toData() const
{
    QByteArray result(size(), 0);
    result[0] = nameSize();
    result[1] = channelNumber();
    result[2] = flags();

    auto dSize = dataSize();
    std::memcpy(&result[3], &dSize, 4); // Размер пакета данных = 4 байта (документация)

    auto chSum = checkSum();
    std::memcpy(&result[7], &chSum, 16); // Контрольная сумма тела пакетах = 16 байта (документация)

    std::memcpy(&result[7 + 16], name().toStdString().c_str(), nameSize());
    std::memcpy(&result[7 + 16 + nameSize()], data().toStdString().c_str(), dataSize());

    return result;
}
