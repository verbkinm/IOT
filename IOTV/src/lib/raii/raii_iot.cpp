#include "raii_iot.h"
#include <cstring>

RAII_iot::RAII_iot()
{
    _iot = nullptr;
}

RAII_iot::RAII_iot(iotv_obj_t *iot)
{
    _iot = iotCopy(iot);
}

RAII_iot::RAII_iot(const RAII_iot &raii_iot)
{
    _iot = iotCopy(raii_iot.iot());
}

RAII_iot::~RAII_iot()
{
    clear_iotv_obj(_iot);
}

iotv_obj_t *RAII_iot::iot() const
{
    return _iot;
}
