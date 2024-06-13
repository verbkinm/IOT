#pragma once

#include <map>
#include <set>

#include "base_host.h"
#include "actions/iotv_action.h"



class IOTV_Event : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enable READ isEnable WRITE setEnable NOTIFY signalEnableChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY signalNameChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY signalHostNameChanged)
    Q_PROPERTY(QString groupName READ group WRITE setGroup NOTIFY signalGroupNameChanged)

    Q_PROPERTY(QString type READ getType CONSTANT)
    Q_PROPERTY(QStringList actionGroups READ actionGroups CONSTANT)

public:
    enum class EVENT_TYPE : uint8_t
    {
        NONE = 0,
        CONNECTING,
        DISCONNECTING,
        STATE,
        DATA,
        ALARM,
        TIMER,

        EVENT_TYPE_NUMBER
    };

    const QString typeName[7] = {
        "NONE",
        "CONNECTING",
        "DISCONNECTING",
        "STATE",
        "DATA",
        "ALARM",
        "TIMER"
    };

    IOTV_Event(EVENT_TYPE type, const Base_Host *host, QObject *parent = nullptr);
    virtual ~IOTV_Event() = default;

    const Base_Host *host() const;

    EVENT_TYPE type() const;
    QString getType() const;

    QString name() const;
    void setName(const QString &newName);

    bool isEnable() const;
    void setEnable(bool newEnable);

    void addAction(std::shared_ptr<IOTV_Action> action);
    void removeAction(const QString &groupName, const QString &actionName);
    void clearActions();

    QStringList actionGroups() const;

    QString group() const;
    void setGroup(const QString &newGroup);

    // Необходим для синхронизации json данных.
    // ключ - группа, значение - список действий группы
    std::map<QString, std::set<QString>> actionMustBeenBinding;

    QString hostName() const;
    void setHostName(const QString &newHostName);

    const Action_List &actions() const;

    friend bool operator<(const IOTV_Event &lhs, const IOTV_Event &rhs);

protected:
    void execActions();

    virtual void runActions(){}
    virtual bool isValid() const {return false;}

    const Base_Host *_host;
    QString _hostName;

private:
    EVENT_TYPE _type;
    Action_List _actions;
    QString _name;
    QString _group;

    bool _enable;

signals:
    void signalEnableChanged(bool);
    void signalNameChanged(QString newName);
    void signalGroupNameChanged(QString newName);
    void signalHostNameChanged(QString newName);

public slots:
    // Вызывается в CurrentConnectionList.qml
    QList<QString> slotActionInGroup(const QString &groupName);

    // удаляет из actionMustBeenBinding. Вызывается в CurrentConnectionList.qml
    void slotRemoveAction(const QString &groupName, const QString &actionName);

    // сохраняетв actionMustBeenBinding. Вызывается в AddCurrentConnection_ActionList.qml
    void slotAddAction(const QString &groupName, const QString &actionName);
};

struct Compare_Event {
    bool operator()(const std::shared_ptr<IOTV_Event> &lhs, const std::shared_ptr<IOTV_Event> &rhs) const
    {
        return *lhs < *rhs;
    }
};

typedef std::set<std::shared_ptr<IOTV_Event>, Compare_Event> Event_List;
