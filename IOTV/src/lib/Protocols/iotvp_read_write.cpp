#include "iotvp_read_write.h"

IOTVP_READ_WRITE::IOTVP_READ_WRITE() :
    _channelNumber(0), _flags(0)
{

}

const QString &IOTVP_READ_WRITE::name() const
{
    return _name;
}

void IOTVP_READ_WRITE::setName(const QString &newName)
{
    _name = newName;
}

uint8_t IOTVP_READ_WRITE::channelNumber() const
{
    return _channelNumber;
}

void IOTVP_READ_WRITE::setChannelNumber(uint8_t newChannelNumber)
{
    _channelNumber = newChannelNumber;
}

uint8_t IOTVP_READ_WRITE::flags() const
{
    return _flags;
}

void IOTVP_READ_WRITE::setFlags(uint8_t newFlags)
{
    _flags = newFlags;
}

const QByteArray &IOTVP_READ_WRITE::data() const
{
    return _data;
}

uint64_t IOTVP_READ_WRITE::size() const
{
    // Чтение / Запись данных (23 + N байт, N максимум 240) (документацию)
    return 23 + nameSize() + dataSize();
}

void IOTVP_READ_WRITE::setData(const QByteArray &newData)
{
    _data = newData;
}

uint8_t IOTVP_READ_WRITE::nameSize() const
{
    return _name.size();
}

uint32_t IOTVP_READ_WRITE::dataSize() const
{
    return _data.size();
}

uint16_t IOTVP_READ_WRITE::checkSum() const
{
    return nameSize() + channelNumber() + flags() + dataSize();
}
