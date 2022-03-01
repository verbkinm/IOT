#ifndef BASE_HOST_H
#define BASE_HOST_H

#include "read_channel.h"
#include "write_channel.h"
#include "Base_Host_global.h"

class BASE_HOST_EXPORT Base_Host : public QObject
{
    Q_OBJECT
public:
    Base_Host(uint8_t id = 0, QObject* parent = nullptr);
    virtual ~Base_Host();

    bool addReadSubChannel(Raw::DATA_TYPE dataType);
    bool addWriteSubChannel(Raw::DATA_TYPE dataType);
    void removeAllSubChannel();

    void setId(uint8_t id);
    void setDescription(const QString &description);
    bool setReadChannelData(size_t channelNumber, Raw::RAW rawData);

    uint8_t getId() const;
    QString getDescription() const;
    Raw::DATA_TYPE getReadChannelDataType(uint8_t channelNumber) const;

    Raw::RAW getReadChannelData(uint8_t channelNumber) const;
    Raw::DATA_TYPE getWriteChannelDataType(uint8_t channelNumber) const;

    int readChannelLength() const;
    int writeChannelLength() const;

    virtual QString getName() const = 0;
    virtual bool getState() const = 0;
    virtual void setState(bool state) = 0;
    virtual qint64 readData(uint8_t channelNumber) = 0;
    virtual qint64 writeData(uint8_t channelNumber, Raw::RAW &rawData) = 0;
    virtual void dataResived(QByteArray data) = 0;

    virtual qint64 writeToServer(QByteArray &data) = 0;

protected:
    uint8_t _id;
    QString _description;

    Read_Channel _readChannel;
    Write_Channel _writeChannel;
};

#endif // BASE_HOST_H
