#pragma once

#include <forward_list>
#include <set>

#include "events/iotv_event.h"
#include "actions/iotv_action.h"

class IOTV_Event_Manager
{
public:
    explicit IOTV_Event_Manager(const Event_List &events, const Action_List &actions,
                                const std::set<QString> &event_groups, const std::set<QString> &action_groups);
    ~IOTV_Event_Manager();

    void bind();

    // Создание EVENT
    //! IOTV_Event_Connect и IOTV_Event_Disconnect
    static IOTV_Event *createEvent(const Base_Host *host, const QString &type);

    //! IOTV_Event_State
    static IOTV_Event *createEvent(const Base_Host *host, const QString &type, const QString &state);

    //! IOTV_Event_Data
    static IOTV_Event *createEvent(const Base_Host *host, const QString &type,
                                   const QString &direction, const QString &data,
                                   const QString &compare, uint8_t channelNumber);

    //! IOTV_Event_Alarm
    static IOTV_Event *createEvent(const QString &type, const QTime &time, const std::array<bool, 7> &days);

    //! IOTV_Event_Timer
    static IOTV_Event *createEvent(const QString &type, int seconds);


    // Создание ACTION
    //! IOTV_Action EMPTY
    static IOTV_Action *createAction();

    //! IOTV_Action_Data_TX
    static IOTV_Action *createAction(const QString &type, Base_Host *host, uint8_t ch_num, const QString &data);

    //! IOTV_Action_Data_TX_Ref
    static IOTV_Action *createAction(const QString &type, Base_Host *dstHost, Base_Host *srcHost,
                                     uint8_t dstCh_num, uint8_t srcCh_Num);

    //
    static IOTV_Event *copyEvent(const IOTV_Event *event);
    static IOTV_Action *copyAction(const IOTV_Action *action);

    // объекты
    std::set<QString> allHostsName() const;

    const Event_List &events() const;
    const Action_List &actions() const;

    Event_List eventsInGroup(const QString &groupName) const;
    Action_List actionsInGroup(const QString &groupName) const;

    void addEvent(std::shared_ptr<IOTV_Event> event);
    void addAction(std::shared_ptr<IOTV_Action> action);

    std::shared_ptr<IOTV_Event> findEvent(const QString &groupName, const QString &eventName) const;
    std::shared_ptr<IOTV_Action> findAction(const QString &groupName, const QString &eventName) const;

    void deleteEvent(const QString &groupName, const QString &eventName);
    void deleteAction(const QString &groupName, const QString &actionName);

    void runEvent(const QString &groupName, const QString &eventName);
    void runAction(const QString &groupName, const QString &actionName);

    // группы
    const std::set<QString> &eventGroups() const;
    const std::set<QString> &actionGroups() const;

    void addEventGroup(const QString &groupName);
    void addActionGroup(const QString &groupName);

    void removeEventGroup(const QString &groupName);
    void removeActionGroup(const QString &groupName);

    void renameEventGroup(const QString &oldGroupName, const QString &newGroupName);
    void renameActionGroup(const QString &oldGroupName, const QString &newGroupName);

private:
    Event_List _events;
    Action_List _actions;

    std::set<QString> _event_groups;
    std::set<QString> _action_groups;
};
