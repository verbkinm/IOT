#include "wrapper_device.h"

Wrapper_Device::Wrapper_Device(QObject *parent) : QObject(parent), device(nullptr), _device(nullptr)
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

QString Wrapper_Device::getName() const
{
    return _device->getName();
}

quint8 Wrapper_Device::getId() const
{
    return _device->getId();
}

QString Wrapper_Device::getDescription() const
{
    return _device->getDescription();
}

void Wrapper_Device::connectSignals()
{
    if(device == nullptr)
        return;

    _device = qobject_cast<Device*>(device);
    connect(_device, &Device::signalState, this, &Wrapper_Device::signalState);
    connect(_device, &Device::signalDataReadRecived, this, &Wrapper_Device::signalDataReadRecived);
}

bool Wrapper_Device::getState()
{
    if(device == nullptr)
        return false;

    return qobject_cast<Device*>(device)->getState();
}

qint8 Wrapper_Device::getReadChannelDataI8(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).i8;
}

qint16 Wrapper_Device::getReadChannelDataI16(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).i16;
}

qint32 Wrapper_Device::getReadChannelDataI32(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).i32;
}

qint64 Wrapper_Device::getReadChannelDataI64(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).i64;
}

quint8 Wrapper_Device::getReadChannelDataUI8(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).ui8;
}

quint16 Wrapper_Device::getReadChannelDataUI16(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).ui16;
}

quint32 Wrapper_Device::getReadChannelDataUI32(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).ui32;
}

quint64 Wrapper_Device::getReadChannelDataUI64(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).ui64;
}

float Wrapper_Device::getReadChannelDataFloat(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).f;
}

double Wrapper_Device::getReadChannelDataDouble(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).d;
}

bool Wrapper_Device::getReadChannelDataBool(quint8 channelNumber) const
{
    return _device->getReadChannelData(channelNumber).b;
}

QString Wrapper_Device::getReadChannelDataString(quint8 channelNumber) const
{
    char *ptr = _device->getReadChannelData(channelNumber).str;
    if(ptr != nullptr)
        return ptr;

    return "Файл пуст!";
}

int Wrapper_Device::readChannelLength() const
{
    return _device->readChannelLength();
}

int Wrapper_Device::writeChannelLength() const
{
    return _device->writeChannelLength();
}

void Wrapper_Device::writeDataUI8(quint8 channelNumber, quint8 value)
{
    Raw::RAW raw;
    raw.ui8 = value;
    _device->writeData(channelNumber, raw);
}

void Wrapper_Device::writeDataBool(quint8 channelNumber, bool value)
{
    Raw::RAW raw;
    raw.b = value;
    _device->writeData(channelNumber, raw);
}
