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

private:
    ACTION_TYPE _type;
};

