#pragma once

#include <QString>

class IOTVP_AbstractBody
{
public:
    IOTVP_AbstractBody();
    virtual ~IOTVP_AbstractBody() = default;

    uint8_t nameSize() const;
    uint8_t flags() const;
    virtual uint16_t checkSum() const = 0;
    const QString &name() const;

    void setName(const QString &newName);
    void setFlags(uint8_t newFlags);

    virtual uint32_t dataSize() const = 0;;
    virtual uint64_t size() const = 0;

    virtual QByteArray toData() const = 0;

    enum class FLAGS : uint8_t
    {
        NONE = 0,
        ERROR = 0xFF
    };

private:
    QString _name;
    uint8_t _flags;
};

