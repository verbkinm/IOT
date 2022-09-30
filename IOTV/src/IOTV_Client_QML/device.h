#pragma once

#include <QTimer>

#include "base_host.h"
#include "IOTV_SC.h"

class Device : public Base_Host
{
    Q_OBJECT

    Q_PROPERTY(bool state READ isOnline NOTIFY stateChanged)
    Q_PROPERTY(int id READ getId CONSTANT)
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

    Q_INVOKABLE void setDataFromString(int channelNumber, QString data);
    Q_INVOKABLE QString readData(int channelNumber) const;
    Q_INVOKABLE QString readDataType(int channelNumber) const;
    Q_INVOKABLE QString writeDataType(int channelNumber) const;

    void setReadInterval(int interval);

    friend bool operator==(const Device &lhs, const Device &rhs);

private:
    const QString _name;
    bool _state;

    QTimer _timerRead, _timerState;
    uint _timerReadInterval, _timerStateInterval;

    QByteArray stringToByteArray();

signals:
    void signalQueryRead();
    void signalQueryState();
    void signalQueryWrite(int channelNumber, QByteArray data);
//    void signalDataChanged(uint8_t channelNumber, QByteArray data);
    void stateChanged();
    void signalUpdate();
};

