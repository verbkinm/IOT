#pragma once

#include <QObject>

#include "channel.h"
#include "iotv_server_embedded.h"
#include "iotv_types.h"

class Base_Host : public QObject
{
    Q_OBJECT
public:
    Base_Host(uint16_t id = 0, QObject *parent = nullptr);
    virtual ~Base_Host() = default;

    uint16_t getId() const;
    QString getDescription() const;

    virtual QString getName() const = 0;
    virtual void setName(const QString &name) = 0;

    uint8_t getReadChannelLength() const;
    uint8_t getWriteChannelLength() const;

    Raw::DATA_TYPE getReadChannelType(uint8_t channelNumber) const;
    Raw::DATA_TYPE getWriteChannelType(uint8_t channelNumber) const;

    const QByteArray &getReadChannelData(uint8_t channelNumber) const;
    const Raw &getReadChannelDataRaw(uint8_t channelNumber) const;
    
    void setState(state_t newState);
    state_t state() const;

    iotv_obj_t *convert() const;

    static constexpr uint16_t TIMER_READ_INTERVAL = 1000;
    static constexpr uint16_t TIMER_STATE_INTERVAL = 1000;
    static constexpr uint16_t TIMER_PING_INTERVAL = 1000;

protected:
    bool setReadChannelData(uint8_t channelNumber, const Raw &data);
    bool setReadChannelData(uint8_t channelNumber, const QByteArray &data);

    bool addReadChannelData(uint8_t channelNumber, const QByteArray &data);

    void clearReadChannelData(uint8_t channelNumber);

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
    
    state_t _state;

signals:
    void signalConnected();
    void signalDisconnected();

    void signalIdentRecived();

    void signalStateOnline();
    void signalStateOffline();
    void signalStateChanged(state_t);
    void signalStateUnknow(state_t);

    // получение сырых данных для формирования пакетов
    void signalDataRiceved(QByteArray);

    // сформированные данные для событий/действий
    void signalDataRX(uint8_t channleNumber, QByteArray data);
    void signalDataTX(uint8_t channleNumber, QByteArray data);
    void signalDataChanged(uint8_t channleNumber, QByteArray data);

    // Используетеся для записи данных полученых от клиентов из других потоков
    void signalQueryWrite(int channelNumber, QByteArray data);
};

