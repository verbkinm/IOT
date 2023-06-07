#pragma once

#include "iotv_event.h"
#include "raw.h"

class IOTV_Event_Data : public IOTV_Event
{   
public:
    enum class DATA_DIRECTION : uint8_t
    {
        NONE = 0,
        RX,
        TX,
        ANY,
        CHANGE
    };

    IOTV_Event_Data(const DATA_DIRECTION &direction, QString compare,
                    const Base_Host *host,
                    uint8_t channelNumber, const Raw &raw,
                    QObject *parent = nullptr);

    DATA_DIRECTION type() const;
    const QString &compareStr() const;

    uint8_t channelNumber() const;

    const Raw &data() const;

    Q_INVOKABLE QString directionString() const;

private:
    DATA_DIRECTION _type;
    uint8_t _channelNumber;
    Raw _data;

    std::function<bool(Raw, Raw)> _compare;
    QString _compareStr;

    std::function<bool(Raw, Raw)> createCompare(const QString &compare);

private slots:
    void slotCheckData(uint8_t channleNumber, QByteArray rhs);
};

