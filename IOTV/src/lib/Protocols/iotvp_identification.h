#pragma once

#include <QString>

class IOTVP_Identification
{
public:
    IOTVP_Identification();

    uint8_t nameSize() const;
    uint8_t flags() const;
    virtual uint32_t dataSize() const = 0;

private:
    QString _name;
    uint8_t _flags;

//    QByteArray _data;
};

