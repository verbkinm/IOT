#pragma once

#include <QTimer>

#include "base_host.h"
#include "IOTV_SC.h"

class Device : public Base_Host
{
    Q_OBJECT

public:
    Device() = default;
    Device(const IOTV_SC::DEV_PKG &dev, QObject *parent = nullptr);
    void update(const IOTV_SC::DEV_PKG &pkg);

    virtual QString getName() const override;

    virtual bool isOnline() const override;
    void setState(bool newState);

    bool setData(uint8_t channelNumber, const QByteArray &data);

    void setReadInterval(int interval);

    friend bool operator==(const Device &lhs, const Device &rhs);

private:
    const QString _name;
    bool _state;

    QTimer _timerRead, _timerState;
    uint _timerReadInterval, _timerStateInterval;

signals:
    void signalQueryRead();
    void signalQueryState();
    void signalDataChanged(uint8_t channelNumber, QByteArray data);
};

