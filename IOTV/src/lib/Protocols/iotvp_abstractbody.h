#pragma once

#include <QtEndian>

#include "iotvp_abstract.h"

class IOTVP_AbstractBody : public IOTVP_Abstract
{
public:
    IOTVP_AbstractBody(BODY_TYPE type);
    virtual ~IOTVP_AbstractBody() = default;

    uint8_t nameSize() const;
    FLAGS flags() const;
    const QString &name() const;

    BODY_TYPE bodyType() const;

    void setName(const QString &newName);
    void setFlags(FLAGS newFlags);
//    void setBodyType(BODY_TYPE newBodyType);

//    virtual bool isValid() const = 0;

   const BODY_TYPE _bodyType;

private:
    QString _name;
    FLAGS _flags;
};

