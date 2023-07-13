#pragma once

#include <forward_list>
#include <set>

#include <QObject>

#include "events/iotv_event.h"
#include "actions/iotv_action.h"

class IOTV_Event_Manager : public QObject
{
    Q_OBJECT
public:
    explicit IOTV_Event_Manager(QObject *parent = nullptr);
    ~IOTV_Event_Manager();

    bool bind(const QString &name, IOTV_Event *event, IOTV_Action *action);

    const std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>>  &worker() const;
    size_t size() const;

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

    static IOTV_Event *createEvent(const QVariantMap &event, const std::forward_list<const Base_Host *> &hosts);

    //! IOTV_Action_Data_TX и IOTV_Action_Data_RX
    static IOTV_Action *createAction(const QString &type, Base_Host *host, uint8_t ch_num, const QString &data);

    //! IOTV_Action_Data_TX_Ref и IOTV_Action_Data_RX_Ref
    static IOTV_Action *createAction(const QString &type, Base_Host *dstHost, Base_Host *srcHost,
                                     uint8_t dstCh_num, uint8_t srcCh_Num);

    static IOTV_Action *createAction(const QVariantMap &action, const std::forward_list<const Base_Host *> &hosts);
private:
    std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *>>> _worker;

    mutable std::mutex _workerMutex;

signals:

};
