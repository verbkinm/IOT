#pragma once

#include "iotv_action.h"
#include "base_host.h"

class IOTV_Action_Data_TX_Ref : public IOTV_Action
{
    Q_OBJECT

    Q_PROPERTY(QString srcHost READ srcHostName WRITE setSrcHostName NOTIFY signalSrcHostChanged)
    Q_PROPERTY(QString dstHost READ dstHostName WRITE setDstHostName NOTIFY signalDstHostChanged)

    Q_PROPERTY(int srcChNum READ srcChannelNumber WRITE setSrcChNum NOTIFY signalSrcChNumChanged)
    Q_PROPERTY(int dstChNum READ dstChannelNumber WRITE setDstChNum NOTIFY signalDstChNumChanged)

public:
    IOTV_Action_Data_TX_Ref(Base_Host *dstHost, uint8_t dstChannelNumber,
                            Base_Host *srcHost, uint8_t srcChannelNumber,
                            QObject *parent = nullptr);

    virtual ~IOTV_Action_Data_TX_Ref() = default;

    const Base_Host *dstHost() const;
    const Base_Host *srcHost() const;

    uint8_t dstChannelNumber() const;
    uint8_t srcChannelNumber() const;

    QString srcHostName() const;
    QString dstHostName() const;

    void setSrcChNum(uint8_t ch);
    void setDstChNum(uint8_t ch);

    void setSrcHostName(const QString &newSrcHostName);
    void setDstHostName(const QString &newDstHostName);

    virtual void exec() override;
    virtual bool isValid() const override;

private:
    Base_Host *_dstHost, *_srcHost;
    QString _dstHostName, _srcHostName;
    uint8_t _dstChannelNumber, _srcChannelNumber;

signals:
    void signalSrcHostChanged(QString);
    void signalDstHostChanged(QString);

    void signalSrcChNumChanged(int);
    void signalDstChNumChanged(int);
};

