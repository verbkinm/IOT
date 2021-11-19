#ifndef WRAPPER_DEVICE_H
#define WRAPPER_DEVICE_H

#include "device.h"
#include <QObject>

class Wrapper_Device : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool state READ getState NOTIFY signalState)
public:
    explicit Wrapper_Device(QObject *parent = nullptr);

    Q_INVOKABLE QObject *getDevice() const;
    Q_INVOKABLE void setDevice(QObject *newDevice);

    Q_INVOKABLE QString getName() const;
    Q_INVOKABLE quint8 getId() const;
    Q_INVOKABLE QString getDescription() const;

    Q_INVOKABLE qint8 getReadChannelDataI8(quint8 channelNumber) const;
    Q_INVOKABLE qint16 getReadChannelDataI16(quint8 channelNumber) const;
    Q_INVOKABLE qint32 getReadChannelDataI32(quint8 channelNumber) const;
    Q_INVOKABLE qint64 getReadChannelDataI64(quint8 channelNumber) const;

    Q_INVOKABLE quint8 getReadChannelDataUI8(quint8 channelNumber) const;
    Q_INVOKABLE quint16 getReadChannelDataUI16(quint8 channelNumber) const;
    Q_INVOKABLE quint32 getReadChannelDataUI32(quint8 channelNumber) const;
    Q_INVOKABLE quint64 getReadChannelDataUI64(quint8 channelNumber) const;

    Q_INVOKABLE float getReadChannelDataFloat(quint8 channelNumber) const;
    Q_INVOKABLE double getReadChannelDataDouble(quint8 channelNumber) const;

    Q_INVOKABLE bool getReadChannelDataBool(quint8 channelNumber) const;
    Q_INVOKABLE QString getReadChannelDataString(quint8 channelNumber) const;

    Q_INVOKABLE int readChannelLength() const;
    Q_INVOKABLE int writeChannelLength() const;

    Q_INVOKABLE void writeDataUI8(quint8 channelNumber, quint8 value);
    Q_INVOKABLE void writeDataBool(quint8 channelNumber, bool value);

    QObject *device;

private:
    Device *_device;

signals:
    void signalState(bool state);
    void signalDataReadRecived();

public slots:
    void connectSignals();
    bool getState();
};

#endif // WRAPPER_DEVICE_H
