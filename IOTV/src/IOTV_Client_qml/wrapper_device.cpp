#include "wrapper_device.h"

Wrapper_Device::Wrapper_Device(QObject *parent) : QObject(parent), device(nullptr)
{

}

QObject *Wrapper_Device::getDevice() const
{
    return device;
}

void Wrapper_Device::setDevice(QObject *newDevice)
{
    device = newDevice;
}

void Wrapper_Device::connectSignals()
{
    if(device == nullptr)
        return;

    Device* dev = qobject_cast<Device*>(device);
    connect(dev, &Device::signalState, this, &Wrapper_Device::signalState);
}

bool Wrapper_Device::getState()
{
    if(device == nullptr)
        return false;

    return qobject_cast<Device*>(device)->getState();
}
