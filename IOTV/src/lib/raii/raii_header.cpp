#include "raii_header.h"

#include <cstdlib>
#include <cstring>

RAII_Header::RAII_Header(header_t *header)
{
    if (header == nullptr)
    {
        header = nullptr;
        return;
    }

    _header = header;
    if (_header->pkg != nullptr)
    {
        _header->pkg = malloc(_header->dataSize);
        if (_header->pkg != nullptr)
            std::memcpy(_header->pkg, header->pkg, _header->dataSize);
    }
}

RAII_Header::~RAII_Header()
{
    clearHeader(_header);
}

const header_t *RAII_Header::header() const
{
    return _header;
}
