#pragma once

#include "base_host.h"
#include "actions/iotv_action.h"

class IOTV_Event : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY signalNameChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY signalHostNameChanged)
    Q_PROPERTY(QString groupName READ group WRITE setGroup NOTIFY signalGroupNameChanged)

    Q_PROPERTY(QString type READ getType CONSTANT)

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
    virtual ~IOTV_Event(){};

    const Base_Host *host() const;

    EVENT_TYPE type() const;
    QString getType() const;

    friend bool operator==(const IOTV_Event &lhs, const IOTV_Event &rhs);
    friend bool operator<(const IOTV_Event &lhs, const IOTV_Event &rhs);

    QString name() const;
    void setName(const QString &newName);

    bool isEnable() const;
    void setEnable(bool newEnable);

    void addAction(std::shared_ptr<IOTV_Action> action);
    void clearActions();

    QString group() const;
    void setGroup(const QString &newGroup);

    // 1 - имя группу действия, 2 - имя действия. Имена действий, которые должны находится в _actions.
    std::vector<std::pair<QString, QString>> actionMustBeenBinding;

    QString hostName() const;
    void setHostName(const QString &newHostName);

    const std::vector<std::shared_ptr<IOTV_Action>> &actions() const;

protected:
    void execActions();

    virtual void runActions(){}
    virtual bool isValid() const {return false;}

    const Base_Host *_host;
    QString _hostName;

private:
    EVENT_TYPE _type;
    std::vector<std::shared_ptr<IOTV_Action>> _actions;
    QString _name;
    QString _group;

    bool _enable;

signals:
    void signalNameChanged(QString newName);
    void signalGroupNameChanged(QString newName);
    void signalHostNameChanged(QString newName);
};
