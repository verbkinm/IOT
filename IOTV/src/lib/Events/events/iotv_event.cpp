#include "iotv_event.h"

IOTV_Event::IOTV_Event(EVENT_TYPE type, const Base_Host *host, QObject *parent) :
    QObject(parent),
    _host(host),
    _type(type),
    _enable(false)
{
    if (host != nullptr)
        setHostName(host->getName());
}

IOTV_Event::EVENT_TYPE IOTV_Event::type() const
{
    return _type;
}

QString IOTV_Event::getType() const
{
    return typeName[static_cast<int>(type())];
}

const Base_Host *IOTV_Event::host() const
{
    return _host;
}

void IOTV_Event::execActions()
{
    if (!_enable)
        return;

    for (auto &action : _actions)
    {
        if (action != nullptr)
            action->exec();
    }
}

QList<QString> IOTV_Event::slotActionInGroup(const QString &groupName)
{
    QList<QString> result;

    for (const auto &pair : actionMustBeenBinding)
    {
        if (pair.first == groupName)
            result << pair.second;
    }

    return result;
}

void IOTV_Event::slotRemoveAction(const QString &groupName, const QString &actionName)
{
    actionMustBeenBinding.erase(actionMustBeenBinding.begin(),
                                std::remove_if(actionMustBeenBinding.begin(), actionMustBeenBinding.end(), [&](auto &pair){
                                    return pair.first == groupName && pair.second == actionName;
                                }));
}

const std::vector<std::shared_ptr<IOTV_Action> > &IOTV_Event::actions() const
{
    return _actions;
}

QString IOTV_Event::hostName() const
{
    return _hostName;
}

void IOTV_Event::setHostName(const QString &newHostName)
{
    if (_hostName != newHostName)
    {
        _hostName = newHostName;
        emit signalHostNameChanged(_hostName);
    }
}

QString IOTV_Event::group() const
{
    return _group;
}

void IOTV_Event::setGroup(const QString &newGroup)
{
    if (_group != newGroup)
    {
        _group = newGroup;
        emit signalGroupNameChanged(_group);
    }
}

QString IOTV_Event::name() const
{
    return _name;
}

void IOTV_Event::setName(const QString &newName)
{
    if (_name != newName)
    {
        _name = newName;
        emit signalNameChanged(_name);
    }
}

bool IOTV_Event::isEnable() const
{
    return _enable;
}

void IOTV_Event::setEnable(bool newEnable)
{
    _enable = newEnable;
}

void IOTV_Event::addAction(std::shared_ptr<IOTV_Action> action)
{
    if (action == nullptr)
        return;

    _actions.push_back(action);
    //    actionMustBeenBinding.emplace_back(action->group(), action->name());
}

void IOTV_Event::removeAction(const QString &groupName, const QString &actionName)
{
    _actions.erase(_actions.begin(), std::remove_if(_actions.begin(), _actions.end(), [&](auto &action){
                       return action.get()->group() == groupName && action.get()->name() == actionName;
               }));
}

void IOTV_Event::clearActions()
{
    _actions.clear();
    //    actionMustBeenBinding.clear();
}

QStringList IOTV_Event::actionGroups() const
{
    QStringList result;
    for (const auto &pair : actionMustBeenBinding)
        result << pair.first;

    return result;
}

//bool operator==(const IOTV_Event &lhs, const IOTV_Event &rhs)
//{
//    return (lhs._type == rhs._type &&
//            lhs._host == rhs._host);
//}

//bool operator<(const IOTV_Event &lhs, const IOTV_Event &rhs)
//{
//    return (lhs._host < rhs._host);
//}
