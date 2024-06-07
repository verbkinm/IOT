#pragma once

#include <QObject>

class IOTV_Action : public QObject
{
    Q_OBJECT
public:
    enum class ACTION_TYPE : uint8_t
    {
        NONE = 0,
        DATA_TX,
        DATA_TX_REF,
        CMD_CLIENT,
        MESSAGE
    };

    IOTV_Action(ACTION_TYPE type, QObject *parent = nullptr);
    virtual ~IOTV_Action() = default;

    ACTION_TYPE type() const;

    virtual void exec() = 0;
    virtual bool isValid() const = 0;

    QString name() const;
    void setName(const QString &newName);

    bool isEnable() const;
    void setEnable(bool newEnable);

//    friend bool operator<(const IOTV_Action &lhs, const IOTV_Action &rhs);

    QString group() const;
    void setGroup(const QString &newGroup);

private:
    ACTION_TYPE _type;
    QString _name;
    QString _group;
    bool _enable;
};

