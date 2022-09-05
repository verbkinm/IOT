#pragma once

#include <QTimer>

#include "base_host.h"
#include "IOTV_SC.h"

class Device : public Base_Host
{
    Q_OBJECT

    Q_PROPERTY(bool state READ isOnline NOTIFY stateChanged)
    Q_PROPERTY(int id READ getId NOTIFY signalIdChanged)
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString description READ getDescription CONSTANT)

    Q_PROPERTY(int readChannelLength READ getReadChannelLength CONSTANT)
    Q_PROPERTY(int writeChannelLength READ getWriteChannelLength CONSTANT)

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
    void stateChanged();
    void signalUpdate();

    void signalIdChanged();
};

