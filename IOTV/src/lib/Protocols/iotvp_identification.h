#pragma once

#include "iotvp_abstractbody.h"
#include "raw.h"

class IOTVP_Identification : public IOTVP_AbstractBody
{
public:
    IOTVP_Identification();
    virtual ~IOTVP_Identification() = default;

    uint16_t id() const;
    uint16_t descriptionSize() const;
    uint8_t numberWriteChannel() const;
    uint8_t numberReadChannel() const;
    virtual uint64_t checkSum() const override;

    const QString &description() const;

    //!Установка ID
    void setId(uint16_t newId);
    void setDescription(const QString &newDescription);

    virtual uint64_t size() const override;

    virtual QByteArray toData() const override;

    virtual uint64_t dataSize() const override;

    const QList<Raw::DATA_TYPE> &readChannel() const;
    void setReadChannel(const QList<Raw::DATA_TYPE> &newReadChannel);

    const QList<Raw::DATA_TYPE> &writeChannel() const;
    void setWriteChannel(const QList<Raw::DATA_TYPE> &newWriteChannel);

    bool operator==(const IOTVP_Abstract &obj) const override;

private:
    uint16_t _id;
    QString _description;

    QList<Raw::DATA_TYPE> _readChannel;
    QList<Raw::DATA_TYPE> _writeChannel;
};
