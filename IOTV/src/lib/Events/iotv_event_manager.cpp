#include "iotv_event_manager.h"

#include "event_action_parser.h"
#include "events/iotv_event_connect.h"
#include "events/iotv_event_disconnect.h"
#include "events/iotv_event_state.h"
#include "events/iotv_event_data.h"
#include "events/iotv_event_alarm.h"

#include "actions/iotv_action_data_tx.h"
#include "actions/iotv_action_data_tx_ref.h"
//#include "actions/iotv_action_msg.h"

IOTV_Event_Manager::IOTV_Event_Manager(QObject *parent)
    : QObject{parent}
{

}

IOTV_Event_Manager::~IOTV_Event_Manager()
{
    for (auto &el : _worker)
    {
        auto event = el.second.first;
        auto action = el.second.second;
        if (event != nullptr)
            delete event;
        if (action != nullptr)
            delete action;
    }
}

bool IOTV_Event_Manager::bind(const QString &name, IOTV_Event *event, IOTV_Action *action)
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

    std::lock_guard lg(_workerMutex);

    event->setParent(this);
    action->setParent(this);

    connect(event, &IOTV_Event::signalEvent, action, &IOTV_Action::exec, Qt::UniqueConnection);

    _worker.emplace_front(name, std::make_pair(event, action));

    return true;
}

const std::forward_list<std::pair<QString, std::pair<IOTV_Event *, IOTV_Action *> > > &IOTV_Event_Manager::worker() const
{
    //!!!
//    std::lock_guard lg(_workerMutex);
    return _worker;
}

size_t IOTV_Event_Manager::size() const
{
    std::lock_guard lg(_workerMutex);
    return std::distance(_worker.begin(), _worker.end());
}

IOTV_Event *IOTV_Event_Manager::createEvent(const Base_Host *host, const QString &type)
{
    if (type == Json_Event_Action::TYPE_CONN)
        return new IOTV_Event_Connect(host);
    else if (type == Json_Event_Action::TYPE_DISCONN)
        return new IOTV_Event_Disconnect(host);

    return nullptr;
}

IOTV_Event *IOTV_Event_Manager::createEvent(const Base_Host *host, const QString &type, const QString &state)
{
    if (type != Json_Event_Action::TYPE_STATE)
        return nullptr;

    IOTV_Event_State::STATE_TYPE newState = IOTV_Event_State::STATE_TYPE::NONE;
    if (state == Json_Event_Action::STATE_ONLINE)
        newState = IOTV_Event_State::STATE_TYPE::ONLINE;
    else if (state == Json_Event_Action::STATE_OFFNLINE)
        newState = IOTV_Event_State::STATE_TYPE::OFFLINE;
    else if (state == Json_Event_Action::STATE_SWITCH)
        newState = IOTV_Event_State::STATE_TYPE::SWITCH;
    else
        return nullptr;

    return new IOTV_Event_State(newState, host);
}

IOTV_Event *IOTV_Event_Manager::createEvent(const Base_Host *host, const QString &type, const QString &direction,
                                            const QString &data,
                                            const QString &compare, uint8_t channelNumber)
{
    if (type != Json_Event_Action::TYPE_DATA)
        return nullptr;

    IOTV_Event_Data::DATA_DIRECTION newDirection = IOTV_Event_Data::DATA_DIRECTION::NONE;
    if (direction == Json_Event_Action::DIRECTION_RX)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::RX;
    else if (direction == Json_Event_Action::DIRECTION_TX)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::TX;
    else if (direction == Json_Event_Action::DIRECTION_ANY)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::ANY;
    else if (direction == Json_Event_Action::DIRECTION_CHANGE)
        newDirection = IOTV_Event_Data::DATA_DIRECTION::CHANGE;
    else
        return nullptr;

    if (compare != Json_Event_Action::COMPARE_EQ && compare != Json_Event_Action::COMPARE_NE
        && compare != Json_Event_Action::COMPARE_GR && compare != Json_Event_Action::COMPARE_LS
        && compare != Json_Event_Action::COMPARE_GE && compare != Json_Event_Action::COMPARE_LE
        && compare != Json_Event_Action::COMPARE_ALWAYS_TRUE && compare != Json_Event_Action::COMPARE_ALWAYS_FALSE)
        return nullptr;

    return new IOTV_Event_Data(newDirection, compare, host, channelNumber, data);
}

IOTV_Event *IOTV_Event_Manager::createEvent(const QString &type, const QTime &time, const std::array<bool, 7> &days)
{
    if (type != Json_Event_Action::TYPE_ALARM)
        return nullptr;

    return new IOTV_Event_Alarm(time, days);
}

IOTV_Event *IOTV_Event_Manager::createEvent(const QVariantMap &event, const std::forward_list<const Base_Host *> &hosts)
{
    IOTV_Event *result = nullptr;

    const Base_Host *host = Event_Action_Parser::hostByName(hosts, event[Json_Event_Action::HOST_NAME].toString());

    if (event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_CONN || event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_DISCONN)
    {
        QString type = event[Json_Event_Action::TYPE].toString();
        result = createEvent(host, type);
    }
    else if (event[Json_Event_Action::TYPE] == Json_Event_Action::STATE)
    {
        QString state = event[Json_Event_Action::STATE].toString();
        QString type = event[Json_Event_Action::TYPE].toString();

        result = IOTV_Event_Manager::createEvent(host, type, state);
    }
    else if(event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_DATA)
    {
        QString direction = event[Json_Event_Action::DIRECTION].toString();
        QString compare = event[Json_Event_Action::COMPARE].toString();
        uint8_t ch_num = event[Json_Event_Action::CH_NUM].toString().toInt();
        QString data = event[Json_Event_Action::DATA].toString();
        QString type = event[Json_Event_Action::TYPE].toString();

        result = IOTV_Event_Manager::createEvent(host, type, direction, data, compare, ch_num);
    }
    else if(event[Json_Event_Action::TYPE] == Json_Event_Action::TYPE_ALARM)
    {
        QString timeStr = event[Json_Event_Action::ALARM_TIME].toString();
        QString daysStr = event[Json_Event_Action::ALARM_DAYS].toString();
        QString type = event[Json_Event_Action::TYPE].toString();

        QTime time;
        std::array<bool, 7> days;

        QStringList HM = timeStr.split(':');
        if ( HM.size() != 2 )
            return nullptr;

        bool ok_h, ok_m;
        int h = HM.at(0).toInt(&ok_h);
        int m = HM.at(1).toInt(&ok_m);

        if (ok_h == false || ok_m == false)
            return nullptr;

        time.setHMS(h, m, 0);

        if ( daysStr.size() != 7 )
            return nullptr;

        for (uint i = 0; i < days.size(); ++i)
        {
            auto ch = daysStr.at(i);
            if ( ch != '0' && ch != '1' )
                return nullptr;

            days[i] = QString(ch).toInt();
        }
        result = IOTV_Event_Manager::createEvent(type, time, days);
    }

    return result;
}

IOTV_Action *IOTV_Event_Manager::createAction(const QString &type, Base_Host *host, uint8_t ch_num,
                                              const QString &data)
{
    if (type != Json_Event_Action::TYPE_DATA_TX/* && type != Json_Event_Action::TYPE_DATA_RX*/)
        return nullptr;

    if (type == Json_Event_Action::TYPE_DATA_TX)
        return new IOTV_Action_Data_TX(host, ch_num, data);

    return nullptr;
}

IOTV_Action *IOTV_Event_Manager::createAction(const QString &type, Base_Host *dstHost, Base_Host *srcHost, uint8_t dstCh_num, uint8_t srcCh_Num)
{
    if (type != Json_Event_Action::TYPE_DATA_TX_REF/* && type != Json_Event_Action::TYPE_DATA_RX_REF*/)
        return nullptr;

    if (type == Json_Event_Action::TYPE_DATA_TX_REF)
        return new IOTV_Action_Data_TX_Ref(dstHost, dstCh_num, srcHost, srcCh_Num);

    return nullptr;
}

IOTV_Action *IOTV_Event_Manager::createAction(const QVariantMap &action, const std::forward_list<const Base_Host *> &hosts)
{
    QString type = action[Json_Event_Action::TYPE].toString();
    IOTV_Action *result = nullptr;

    if (type == Json_Event_Action::TYPE_DATA_TX)
    {
        const Base_Host *host = Event_Action_Parser::hostByName(hosts, action[Json_Event_Action::HOST_NAME].toString());
        QString data = action[Json_Event_Action::DATA].toString();
        uint8_t ch_num = action[Json_Event_Action::CH_NUM].toString().toInt();

        result = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(host), ch_num, data);

    }
    else if (type == Json_Event_Action::TYPE_DATA_TX_REF)
    {
        const Base_Host *dstHost = Event_Action_Parser::hostByName(hosts, action[Json_Event_Action::HOST_DST].toString());
        const Base_Host *srcHost = Event_Action_Parser::hostByName(hosts, action[Json_Event_Action::HOST_SRC].toString());

        uint8_t dstChNum = action[Json_Event_Action::CH_NUM_DST].toString().toInt();
        uint8_t srcChNum = action[Json_Event_Action::CH_NUM_SRC].toString().toInt();

        result = IOTV_Event_Manager::createAction(type, const_cast<Base_Host *>(dstHost), const_cast<Base_Host *>(srcHost), dstChNum, srcChNum);
    }

    return result;
}
