#pragma once

#include <list>

#include <QObject>

#include "events/iotv_event_connect.h"
#include "events/iotv_event_disconnect.h"
#include "events/iotv_event_state.h"
#include "events/iotv_event_data.h"

#include "actions/iotv_action_data_tx.h"
#include "actions/iotv_action_data_tx_ref.h"
#include "actions/iotv_action_msg.h"

#include "ConfigTypes.h"

class IOTV_Event_Manager : public QObject
{
    Q_OBJECT
public:
    explicit IOTV_Event_Manager(QObject *parent = nullptr);

    bool bind(IOTV_Event *event, IOTV_Action *action);

    const std::list<std::pair<IOTV_Event *, IOTV_Action *> > &worker() const;
    size_t size() const;

    IOTV_Event *createEvent(Base_Host *host, const QString &type) const;
    IOTV_Event *createEvent(Base_Host *host, const QString &type, const QString &state) const;
    IOTV_Event *createEvent(Base_Host *host, const QString &type, const QString &direction,
                           const QVariant &variant, const QString &compare, uint8_t channelNumber) const;

    std::function<bool(Raw, Raw)> createCompare(const QString &compare) const;

    IOTV_Action *createAction() const;

private:

    std::list<std::pair<IOTV_Event *, IOTV_Action *>> _worker;

signals:

};
