#pragma once

#include "header.h"

class RAII_Header
{
public:
    RAII_Header(header_t *header);
    ~RAII_Header();

    const header_t *header() const;

private:
    header_t *_header;
};

