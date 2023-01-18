#pragma once

#include <QtEndian>
//#include <QString>
//#include <cstring>

class IOTVP_AbstractBody
{
public:
    IOTVP_AbstractBody();
    virtual ~IOTVP_AbstractBody() = default;

    enum class BODY_TYPE : uint8_t
    {
        NONE = 0,
        IDENTIFICATION,
        STATE,
        READ_WRITE
    };

    enum class FLAGS : uint8_t
    {
        NONE = 0,
        ERROR = 0xFF
    };

    uint8_t nameSize() const;
    FLAGS flags() const;
    virtual uint64_t checkSum() const = 0;
    const QString &name() const;


    virtual uint32_t dataSize() const = 0;;
    virtual uint64_t size() const = 0;

    virtual QByteArray toData() const = 0;

    BODY_TYPE bodyType() const;

    void setName(const QString &newName);
    void setFlags(FLAGS newFlags);
    void setBodyType(BODY_TYPE newBodyType);

    BODY_TYPE _bodyType;

private:
    QString _name;
    FLAGS _flags;
};

