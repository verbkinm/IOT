#pragma once

#include <mutex>

#include <QThread>

#include "read_channel.h"
#include "write_channel.h"
#include "Base_Host_global.h"

class BASE_HOST_EXPORT Base_Host : public QObject
{
    Q_OBJECT
public:
    Base_Host(uint8_t id = 0, QObject* parent = nullptr);
    virtual ~Base_Host();


    bool addWriteSubChannel(Raw::DATA_TYPE dataType);
    void removeAllSubChannel();

    void setId(uint8_t id);
    void setDescription(const QString &description);
    bool setReadChannelData(size_t channelNumber, Raw::RAW rawData);

    uint8_t getId() const;
    QString getDescription() const;

    Raw::DATA_TYPE getReadChannelDataType(uint8_t channelNumber) const;
    Raw::DATA_TYPE getWriteChannelDataType(uint8_t channelNumber) const;

    Raw::RAW getReadChannelData(uint8_t channelNumber) const;

    int readChannelLength() const;
    int writeChannelLength() const;

    virtual QString getName() const = 0;

    virtual bool isOnline() const = 0;
    virtual void setOnline(bool state) = 0;

    virtual qint64 readData(uint8_t channelNumber) = 0;
    virtual qint64 writeData(uint8_t channelNumber, Raw::RAW &rawData) = 0;
    virtual void dataResived(QByteArray data) = 0;

    virtual qint64 writeToServer(QByteArray &data) = 0;

    virtual bool runInNewThread() = 0;

protected:
    bool addReadSubChannel(Raw::DATA_TYPE dataType);

    uint8_t _id;
    QString _description;

    Read_Channel _readChannel;
    Write_Channel _writeChannel;

    std::mutex _mutexParametersChange;
    QThread _thread;

    friend class IOTV_SH;
};

