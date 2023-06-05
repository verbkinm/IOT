#pragma once

#include <QTimer>

#include "base_host.h"
#include "IOTV_SH.h"
#include "raw.h"
#include "iotv_server_embedded.h"

class Device : public Base_Host
{
    Q_OBJECT

    Q_PROPERTY(bool state READ isOnline NOTIFY signalStateChanged)
    Q_PROPERTY(int id READ getId CONSTANT)
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString description READ getDescription CONSTANT)
    Q_PROPERTY(QString aliasName READ aliasName WRITE setAliasName NOTIFY signalAliasNameChanged)

    Q_PROPERTY(int readChannelLength READ getReadChannelLength CONSTANT)
    Q_PROPERTY(int writeChannelLength READ getWriteChannelLength CONSTANT)

public:
    Device() = default;
    Device(const struct IOTV_Server_embedded *dev, QObject *parent = nullptr);
    void update(const struct IOTV_Server_embedded *dev);

    virtual QString getName() const override;

    bool isOnline() const;
    void setState(bool newState);

    bool setData(uint8_t channelNumber, const QByteArray &data);

    Q_INVOKABLE void setDataFromString(int channelNumber, QString data);
    Q_INVOKABLE QString readData(int channelNumber) const;
    Q_INVOKABLE QString readDataType(int channelNumber) const;
    Q_INVOKABLE QString writeDataType(int channelNumber) const;

    void setReadInterval(int interval);

    friend bool operator==(const Device &lhs, const Device &rhs);
    friend bool operator!=(const Device &lhs, const Device &rhs); // для android версий

    const QString &aliasName() const;
    void setAliasName(const QString &newAliasName);

private:
    const QString _name;
    QString _aliasName;

    QTimer _timerRead, _timerState;

//    QByteArray stringToByteArray();

signals:
    void signalQueryIdentification();
    void signalQueryRead();
    void signalQueryState();
    void signalQueryWrite(int channelNumber, QByteArray data);

    void signalStateChanged();
    void signalUpdate();
    void signalAliasNameChanged();

    void signalDataChanged(int channel);

private slots:
    void slotTimerReadTimeOut();
    void slotTimerStateTimeOut();
};

