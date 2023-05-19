#pragma once

#include <list>

#include <QObject>

#include "events/iotv_event_connect.h"
#include "events/iotv_event_disconnect.h"
#include "events/iotv_event_state.h"
#include "events/iotv_event_data.h"

#include "actions/iotv_action_data_tx.h"
#include "actions/iotv_action_msg.h"

class IOTV_Event_Manager : public QObject
{
    Q_OBJECT
public:
    IOTV_Event_Manager(QObject *parent = nullptr);
    bool bind(IOTV_Event *event, IOTV_Action *action);

    const std::list<std::pair<IOTV_Event *, IOTV_Action *> > &worker() const;

private:
    std::list<std::pair<IOTV_Event *, IOTV_Action *>> _worker;

signals:

};
