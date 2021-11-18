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

    QObject *device;

signals:
    void signalState(bool state);

public slots:
    void connectSignals();
    bool getState();
};

#endif // WRAPPER_DEVICE_H
