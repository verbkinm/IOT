#pragma once

#include "header.h"

#include <QObject>

class RAII_Header
{
public:
    RAII_Header();
    RAII_Header(header_t *header);
//    RAII_Header(RAII_Header &&raii_header);
    RAII_Header(const RAII_Header &raii_header);
    ~RAII_Header();

    header_t *header() const;

private:
    header_t *_header;
};

Q_DECLARE_METATYPE(RAII_Header);

