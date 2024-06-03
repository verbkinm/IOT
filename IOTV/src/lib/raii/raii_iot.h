#pragma once

#include "iotv_server_embedded.h"

#include <QObject>

class RAII_iot
{
public:
    RAII_iot();
    RAII_iot(iotv_obj_t *iot);
    RAII_iot(const RAII_iot &raii_iot);
    ~RAII_iot();

    iotv_obj_t *iot() const;

private:
    iotv_obj_t *_iot;
};

Q_DECLARE_METATYPE(RAII_iot);
