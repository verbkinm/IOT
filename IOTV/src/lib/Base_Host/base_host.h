#pragma once

#include <QObject>

#include "channel.h"
#include "creatorpkgs.h"
#include "IOTV_SH.h"

class Base_Host : public QObject
{
    Q_OBJECT
public:
    Base_Host(uint16_t id = 0, QObject *parent = nullptr);
    virtual ~Base_Host() = default;

    uint16_t getId() const;
    QString getDescription() const;

    virtual QString getName() const = 0;

    uint8_t getReadChannelLength() const;
    uint8_t getWriteChannelLength() const;

    Raw::DATA_TYPE getReadChannelType(uint8_t channelNumber) const;
    Raw::DATA_TYPE getWriteChannelType(uint8_t channelNumber) const;

    const QByteArray &getReadChannelData(uint8_t channelNumber) const;
    const Raw &getReadChannelDataRaw(uint8_t channelNumber) const;

    void setState(State_STATE newState);
    State_STATE state() const;

    struct IOTV_Server_embedded *convert() const;

    static constexpr uint16_t TIMER_READ_INTERVAL = 1000;
    static constexpr uint16_t TIMER_STATE_INTERVAL = 1000;
    static constexpr uint16_t TIMER_PING_INTERVAL = 1000;

protected:
    bool setReadChannelData(uint8_t channelNumber, const Raw &data);
    bool setReadChannelData(uint8_t channelNumber, const QByteArray &data);

    bool setWriteChannelData(uint8_t channelNumber, const Raw &data);

    void setReadChannel(const Channel &newReadChannel);
    void setWriteChannel(const Channel &newWriteChannel);

    void addWriteSubChannel(const Raw &data);
    void addReadSubChannel(const Raw &data);

    void removeAllSubChannel();

    void setId(uint16_t id);
    void setDescription(const QString &description);

private:
    uint16_t _id;
    QString _description;

    Channel _readChannel;
    Channel _writeChannel;

    State_STATE _state;

signals:
    void signalConnected();
    void signalDisconnected();

    void signalIdentRecived();

    void signalStateOnline();
    void signalStateOffline();
    void signalStateChanged(State_STATE);
    void signalStateUnknow(State_STATE);

    // получение сырых данных для формирования пакетов
    void signalDataRiceved(QByteArray);

    // сформированные данные
    void signalDataRX(uint8_t channleNumber, QByteArray data);
    void signalDataTX(uint8_t channleNumber, QByteArray data);
    void signalDataChanged(uint8_t channleNumber, QByteArray data);

    // Используетеся для записи данных полученых от клиентов из других потоков
    void signalQueryWrite(int channelNumber, QByteArray data);
};

