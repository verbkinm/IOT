#include "iotv_event.h"

IOTV_Event::IOTV_Event(EVENT_TYPE type, const Base_Host *host, QObject *parent) :
    QObject(parent),
    _host(host),
    _type(type)
{

}

IOTV_Event::EVENT_TYPE IOTV_Event::type() const
{
    return _type;
}

const Base_Host *IOTV_Event::host() const
{
    return _host;
}

bool IOTV_Event::isValid() const
{
    if (_host == nullptr || _type == EVENT_TYPE::NONE)
        return false;

    return true;
}

bool operator==(const IOTV_Event &lhs, const IOTV_Event &rhs)
{
    return (lhs._type == rhs._type &&
            lhs._host == rhs._host);
}

bool operator<(const IOTV_Event &lhs, const IOTV_Event &rhs)
{
    return (lhs._host < rhs._host);
}