#pragma once

#include <QObject>

class IOTV_Action : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enable READ isEnable WRITE setEnable NOTIFY signalEnableChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY signalNameChanged)
    Q_PROPERTY(QString groupName READ group WRITE setGroup NOTIFY signalGroupNameChanged)

    Q_PROPERTY(QString type READ getType CONSTANT)

public:
    enum class ACTION_TYPE : uint8_t
    {
        NONE = 0,
        DATA_TX,
        DATA_TX_REF,
        CMD_CLIENT,
        MESSAGE
    };

    const QString typeName[5] = {
        "NONE",
        "DATA_TX",
        "DATA_TX_REF",
        "CMD_CLIENT",
        "MESSAGE"
    };

    IOTV_Action(ACTION_TYPE type, QObject *parent = nullptr);
    virtual ~IOTV_Action() = default;

    ACTION_TYPE type() const;
    QString getType() const;

    virtual void exec() {};
    virtual bool isValid() const {return false;};

    QString name() const;
    void setName(const QString &newName);

    bool isEnable() const;
    void setEnable(bool newEnable);

    QString group() const;
    void setGroup(const QString &newGroup);

private:
    ACTION_TYPE _type;
    QString _name;
    QString _group;
    bool _enable;

signals:
    void signalEnableChanged(bool);
    void signalNameChanged(QString newName);
    void signalGroupNameChanged(QString newName);
};

