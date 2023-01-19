#pragma once

#include "iotvp_header.h"
#include "iotvp_abstractbody.h"
#include "iotvp_identification.h"
#include "iotvp_state.h"
#include "iotvp_read_write.h"

class IOTVP_Creator
{
public:
    IOTVP_Creator() = delete;
    IOTVP_Creator(QByteArray &data);

    std::pair<std::unique_ptr<IOTVP_Header>, std::unique_ptr<IOTVP_AbstractBody> > createPkg();

    bool error() const;

    uint64_t expectedDataSize() const;

private:
    void createHeader() const;
    void createBody() const;
    void createBodyIdentification() const ;
    void createBodyState() const;
    void createBodyReadWrite() const;

    bool bodyMustBe(IOTVP_Header::ASSIGNMENT assigment) const;

    mutable bool _error, _complete;

    QByteArray &_rawData;

    mutable std::unique_ptr<IOTVP_Header> _header;
    std::unique_ptr<IOTVP_AbstractBody> _body;

    uint64_t _expectedDataSize;
};

