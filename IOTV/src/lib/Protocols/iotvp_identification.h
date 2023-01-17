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
    virtual uint16_t checkSum() const override;

    const QString &description() const;

    void setId(uint16_t newId);
    void setDescription(const QString &newDescription);

    virtual uint64_t size() const override;

    virtual QByteArray toData() const override;

private:
    uint16_t _id;
    QString _description;

    std::vector<Raw::DATA_TYPE> _readChannel;
    std::vector<Raw::DATA_TYPE> _writeChannel;

};

