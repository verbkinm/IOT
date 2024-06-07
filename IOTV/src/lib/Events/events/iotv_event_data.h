#pragma once

#include "iotv_event.h"
#include "raw.h"

class IOTV_Event_Data : public IOTV_Event
{
    Q_OBJECT

    Q_PROPERTY(QString directionStr READ getDirection WRITE setDirection NOTIFY signalDirectionChanged)
    Q_PROPERTY(QString compare READ compareStr WRITE setCompareStr NOTIFY signalCompareChanged)
    Q_PROPERTY(QString dataStr READ data WRITE setDataStr NOTIFY signalDataChanged)
    Q_PROPERTY(int chNum READ channelNumber WRITE setChannelNumber NOTIFY signalChannelNumberChanged)

public:
    enum class DATA_DIRECTION : uint8_t
    {
        NONE = 0,
        RX,
        TX,
        ANY,
        CHANGE,

        DATA_DIRECTION_NUMBER
    };

    const QString directionType[5] = {
        "NONE",
        "RX",
        "TX",
        "ANY",
        "CHANGE"
    };

    IOTV_Event_Data(const DATA_DIRECTION &direction, const QString &compare,
                    const Base_Host *host,
                    uint8_t channelNumber, const QString &data,
                    QObject *parent = nullptr);

    DATA_DIRECTION direction() const;
    QString getDirection() const;
    QString compareStr() const;
    uint8_t channelNumber() const;
    QString data() const;

    void setDirection(const QString &newDirection);
    void setCompareStr(const QString &newCompare);
    void setDataStr(const QString &newDataStr);
    void setChannelNumber(uint8_t newChNum);

private:
    virtual void runActions() override;
    virtual bool isValid() const override;

    DATA_DIRECTION _type;
    uint8_t _channelNumber;
    QString _data;

    QString _compareStr;
    std::function<bool(Raw, Raw)> _compare;

    std::function<bool(Raw, Raw)> createCompare(const QString &compare);

private slots:
    void slotCheckData(uint8_t channleNumber, QByteArray rhs);

signals:
    void signalDirectionChanged(QString);
    void signalCompareChanged(QString);
    void signalDataChanged(QString);
    void signalChannelNumberChanged(int);
};

