#pragma once

#include "iotv_server_embedded.h"

class RAII_iot
{
public:
    RAII_iot(iotv_obj_t *iot);
    ~RAII_iot();

    const iotv_obj_t *iot() const;

private:
    iotv_obj_t *_iot;
};

