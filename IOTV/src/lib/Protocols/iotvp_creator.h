#pragma once

#include "iotvp_header.h"
#include "iotvp_identification.h"
#include "iotvp_state.h"
#include "iotvp_read_write.h"

class IOTVP_Creator
{
public:
    IOTVP_Creator() = delete;
    IOTVP_Creator(const QByteArray &data);

    void createPkgs();

    bool error() const;

    uint64_t expectedDataSize() const;
    uint64_t cutDataSize() const;

    std::unique_ptr<IOTVP_Header> takeHeader();
    std::unique_ptr<IOTVP_AbstractBody> takeBody();

    bool bodyMustBe(IOTVP_Header::ASSIGNMENT assigment) const;

private:
    void createHeader();
    void createBody();
    void createBodyIdentification();
    void createBodyState();
    void createBodyReadWrite();

    bool _error;
    uint64_t _expectedDataSize,
    _cutDataSize,
    // Для страховочной проверки
    _headerCheckSumMustBe,
    _headerDataSizeMustBe,
    _bodyCheckSumMustBe,
    _bodyDataSizeMustBe;

    const QByteArray &_rawData;

    std::unique_ptr<IOTVP_Header> _header;
    std::unique_ptr<IOTVP_AbstractBody> _body;
};

