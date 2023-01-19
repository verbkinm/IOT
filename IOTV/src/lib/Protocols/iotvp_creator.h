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

    void createPkgs();

    bool error() const;
    bool complete() const;

    uint64_t expectedDataSize() const;

    std::unique_ptr<IOTVP_Header> takeHeader();
    std::unique_ptr<IOTVP_AbstractBody> takeBody();

private:
    void createHeader();
    void createBody();
    void createBodyIdentification();
    void createBodyState();
    void createBodyReadWrite();

    bool bodyMustBe(IOTVP_Header::ASSIGNMENT assigment) const;

    bool _error, _complete;
    uint64_t _expectedDataSize;

    QByteArray &_rawData;

    std::unique_ptr<IOTVP_Header> _header;
    std::unique_ptr<IOTVP_AbstractBody> _body;

};

