#include "iotv_action.h"

IOTV_Action::IOTV_Action(ACTION_TYPE type, QObject *parent) :
    QObject(parent),
    _type(type)
{

}

IOTV_Action::ACTION_TYPE IOTV_Action::type() const
{
    return _type;
}
