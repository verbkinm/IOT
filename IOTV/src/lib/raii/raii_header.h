#pragma once

#include "header.h"

#include <QObject>

class RAII_Header
{
public:
    RAII_Header();
    explicit RAII_Header(header_t *header);
    RAII_Header(const RAII_Header &raii_header);
    ~RAII_Header();

    RAII_Header &operator=(const RAII_Header &raii_header);

    header_t *header() const;

    size_t toData(QByteArray &data) const;

private:
    header_t *_header;
};

Q_DECLARE_METATYPE(RAII_Header);

