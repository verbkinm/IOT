#include "raii_header.h"

#include <cstdlib>
#include <cstring>

RAII_Header::RAII_Header()
{
    _header = nullptr;
}

RAII_Header::RAII_Header(header_t *header)
{
    _header = headerCopy(header);
}

RAII_Header::RAII_Header(const RAII_Header &raii_header)
{
    _header = headerCopy(raii_header.header());
}

RAII_Header::~RAII_Header()
{
    clearHeader(_header);
}

RAII_Header &RAII_Header::operator=(const RAII_Header &raii_header)
{
    if (&raii_header == this)
        return *this;

    clearHeader(_header);
    _header = headerCopy(raii_header.header());

    return *this;
}

header_t *RAII_Header::header() const
{
    return _header;
}

size_t RAII_Header::toData(QByteArray &data) const
{
    data.resize(headerSize(_header));

    return headerToData(_header, data.data(), data.size());
}
