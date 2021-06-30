#include <QApplication>

#include "device.h"
#include "server.h"

static int readInterval = 5000;

Device::Device(Server &server, const QString &name, uint8_t id, QObject *parent) : Base_Host(id, parent),
    _name(name), _viewName(name), _state(false), _server(server)
{
    newObjectName();

    _description = "description"; // !!!

    connect(&_autoReadInterval, &QTimer::timeout, this, &Device::slotAutoReadTimeOut);
    connect(&_stateInterval, &QTimer::timeout, this, &Device::slotStateIntervalTimeOut);

    _autoReadInterval.stop();
    _autoReadInterval.setInterval(readInterval);
    _stateInterval.start(5000);
}

Device::~Device()
{
    destroyObservers();
}

QString Device::getName() const
{
    return _name;
}

bool Device::getState() const
{
    return _state;
}

int64_t Device::readData(uint8_t channelNumber)
{
    _autoReadInterval.stop();
    int64_t res =  Protocols::SC.query_READ(*this, _name, channelNumber);
    _autoReadInterval.start();
    return res;
}

int64_t Device::writeData(uint8_t channelNumber, Raw::RAW &rawData)
{
    return IOTV_SC::query_WRITE(*this, _name, channelNumber, rawData);
}

void Device::dataResived(QByteArray data)
{
    IOTV_SC::Response_Type dataType = IOTV_SC::checkResponsetData(data);
    if(dataType == IOTV_SC::Response_Type::RESPONSE_STATE)
        IOTV_SC::serverResponse_STATE(*this, data);
    else if(dataType == IOTV_SC::Response_Type::RESPONSE_READ)
        IOTV_SC::serverResponse_READ(*this, data);
    else if(dataType == IOTV_SC::Response_Type::RESPONSE_WRITE)
        IOTV_SC::serverResponse_WRITE(*this, data);
    else
        return;

    qApp->processEvents();
    notify();
}

void Device::setState(bool state)
{
    _state = state;
}

int Device::getAutoReadInterval() const
{
    return _autoReadInterval.interval();
}

void Device::setAutoReadInterval(int value)
{
    _autoReadInterval.setInterval(value);
}

qint64 Device::writeToServer(QByteArray &data)
{
    //    Log::write(_name + ": Data send to " + _server.getServerAddress() + ":"
    //               + QString::number(_server.getServerPort())
    //               + " -> " + data.toHex(':'));
    return _server.writeData(data);
}

void Device::autoReadEnable(bool state)
{
    if(state)
        _autoReadInterval.start();
    else
        _autoReadInterval.stop();
}

QString Device::getViewName() const
{
    return _viewName;
}

void Device::setViewName(const QString &viewName)
{
    _viewName = viewName;
    newObjectName();
    _server.addAlias(_name, viewName);
}

void Device::newObjectName()
{
    this->setObjectName(_viewName + " (" + _name + ")");
}

const Server &Device::getServer() const
{
    return _server;
}

void Device::slotAutoReadTimeOut()
{
    for (uint8_t i = 0; i < readChannelLength(); i++)
        Protocols::SC.query_READ(*this, _name, i);

//    notify();
}

void Device::slotStateIntervalTimeOut()
{
    QByteArray data;
    if(IOTV_SC::query_STATE(data, _name))
        _server.writeData(data);
}
