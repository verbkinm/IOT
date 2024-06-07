#pragma once

#include <QObject>
#include "iotv_event.h"

class IOTV_Event_State : public IOTV_Event
{
    Q_OBJECT

    Q_PROPERTY(QString state READ getState WRITE setState NOTIFY signalStateSchanged)

public:
    enum class STATE_TYPE : uint8_t
    {
        NONE = 0,
        ONLINE,
        OFFLINE,
        SWITCH,

        STATE_TYPE_NUMBER
    };

    const QString stateName[4] = {
        "NONE",
        "ONLINE",
        "OFFLINE",
        "SWITCH"
    };

    IOTV_Event_State(STATE_TYPE state, const Base_Host *host,
                     QObject *parent = nullptr);

    STATE_TYPE state() const;
    QString getState() const;
    void setState(const QString &state);

private:
    virtual void runActions() override;
    virtual bool isValid() const override;

    STATE_TYPE _state;

signals:
    void signalStateSchanged(QString state);
};

