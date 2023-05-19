#include "iotv_event_manager.h"

IOTV_Event_Manager::IOTV_Event_Manager(QObject *parent)
    : QObject{parent}
{

}

bool IOTV_Event_Manager::bind(IOTV_Event *event, IOTV_Action *action)
{
    if (event == nullptr && action == nullptr)
        return false;
    if (event == nullptr && action != nullptr)
    {
        delete action;
        return false;
    }
    if (event != nullptr && action == nullptr)
    {
        delete event;
        return false;
    }

    if (!event->isValid() || !action->isValid())
    {
        delete event;
        delete action;
        return false;
    }

    event->setParent(this);
    action->setParent(this);

    connect(event, &IOTV_Event::signalEvent, action, &IOTV_Action::exec, Qt::UniqueConnection);
    _worker.emplace_back(event, action);

    return true;
}

const std::list<std::pair<IOTV_Event *, IOTV_Action *> > &IOTV_Event_Manager::worker() const
{
    return _worker;
}
