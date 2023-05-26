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


class IOTV_Event_Manager : public QObject
{
    Q_OBJECT
public:
    explicit IOTV_Event_Manager(QObject *parent = nullptr);

    bool bind(IOTV_Event *event, IOTV_Action *action);

    const std::list<std::pair<IOTV_Event *, IOTV_Action *> > &worker() const;
    size_t size() const;

    //! IOTV_Event_Connect и IOTV_Event_Disconnect
    IOTV_Event *createEvent(Base_Host *host, const QString &type) const;

    //! IOTV_Event_State
    IOTV_Event *createEvent(Base_Host *host, const QString &type, const QString &state) const;

    //! IOTV_Event_Data
    IOTV_Event *createEvent(Base_Host *host, const QString &type, const QString &direction,
                            const QByteArray &data, const QString &dataType,
                            const QString &compare, uint8_t channelNumber) const;

    std::function<bool(Raw, Raw)> createCompare(const QString &compare) const;

    //! IOTV_Action_Data_TX и IOTV_Action_Data_RX
    IOTV_Action *createAction(const QString &type, Base_Host *host, uint8_t ch_num,
                              const QByteArray &data, const QString &dataType) const;

    //! IOTV_Action_Data_TX_Ref и IOTV_Action_Data_RX_Ref
    IOTV_Action *createAction(const QString &type, Base_Host *dstHost, Base_Host *srcHost,
                              uint8_t dstCh_num, uint8_t srcCh_Num) const;

private:

    std::list<std::pair<IOTV_Event *, IOTV_Action *>> _worker;

signals:

};
