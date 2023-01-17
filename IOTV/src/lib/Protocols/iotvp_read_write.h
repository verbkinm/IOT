#pragma once

#include <QString>

class IOTVP_READ_WRITE
{
public:
    IOTVP_READ_WRITE();

    uint8_t nameSize() const;
    uint8_t channelNumber() const;
    uint8_t flags() const;
    uint32_t dataSize() const;
    uint16_t checkSum() const;
    const QString &name() const;
    const QByteArray &data() const;

    uint64_t size() const;

    void setChannelNumber(uint8_t newChannelNumber);
    void setFlags(uint8_t newFlags);
    void setData(const QByteArray &newData);
    void setName(const QString &newName);

private:
    QString _name;
    uint8_t _channelNumber;
    uint8_t _flags;

    QByteArray _data;
};
