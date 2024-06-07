#include "iotv_action.h"

IOTV_Action::IOTV_Action(ACTION_TYPE type, QObject *parent) :
    QObject(parent),
    _type(type),
    _enable(false)
{

}

IOTV_Action::ACTION_TYPE IOTV_Action::type() const
{
    return _type;
}

QString IOTV_Action::name() const
{
    return _name;
}

void IOTV_Action::setName(const QString &newName)
{
    _name = newName;
}

bool IOTV_Action::isEnable() const
{
    return _enable;
}

void IOTV_Action::setEnable(bool newEnable)
{
    if (isValid())
        _enable = newEnable;
}

QString IOTV_Action::group() const
{
    return _group;
}

void IOTV_Action::setGroup(const QString &newGroup)
{
    _group = newGroup;
}

//bool operator<(const IOTV_Action &lhs, const IOTV_Action &rhs)
//{
//    return (std::make_tuple(lhs.group(), lhs.name()) < std::make_tuple(rhs.group(), rhs.name()));
//}
