#pragma once

#include "iotv_action.h"
#include "base_host.h"

class IOTV_Action_Data_TX : public IOTV_Action
{
    Q_OBJECT

    Q_PROPERTY(QString dataStr READ data WRITE setDataStr NOTIFY signalDataChanged)
    Q_PROPERTY(int chNum READ channelNumber WRITE setChannelNumber NOTIFY signalChannelNumberChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY signalHostNameChanged)

public:
    IOTV_Action_Data_TX(Base_Host *host, uint8_t channelNumber, const QString &data, QObject *parent = nullptr);
    virtual ~IOTV_Action_Data_TX() = default;

    const Base_Host *host() const;

    QString hostName() const;
    QString data() const;
    uint8_t channelNumber() const;

    void setHostName(const QString &newHostName);
    void setDataStr(const QString &newDataStr);
    void setChannelNumber(uint8_t newChNum);

    virtual void forceExec() override;
    virtual void exec() override;
    virtual bool isValid() const override;

private:
    Base_Host *_host;
    QString _hostName;
    uint8_t _channelNumber;
    QString _data;

signals:
    void signalHostNameChanged(QString);
    void signalDataChanged(QString);
    void signalChannelNumberChanged(int);
};

