#include "client.h"

Client::Client(const QString &address, const quint16 &port, QObject *parent) : QObject{parent},
    _address{address}, _port{port}
{
    connect(&_socket, &QTcpSocket::connected, this, &Client::slotConnected);
    connect(&_socket, &QTcpSocket::disconnected, this, &Client::slotDisconnected);
    connect(&_socket, &QTcpSocket::readyRead, this, &Client::slotReciveData);

    connect(&_timerDevList, &QTimer::timeout, this, &Client::slotQueryDevList);
    _timerDevList.start(5000);
}

Client::~Client()
{

}

void Client::connectToHost()
{
    _socket.connectToHost(_address, _port);
}

void Client::disconnectFromHost()
{
    _socket.disconnectFromHost();
}

qint64 Client::writeData(const QByteArray &data)
{
    return _socket.write(data);
}

const QString &Client::address() const
{
    return _address;
}

void Client::setAddress(const QString &newAddress)
{
    if (_address == newAddress)
        return;
    _address = newAddress;

    emit addressChanged();
}

quint16 Client::port() const
{
    return _port;
}

void Client::setPort(quint16 newPort)
{
    if (_port == newPort)
        return;
    _port = newPort;

    emit portChanged();
}

void Client::response_DEV_LIST(IOTV_SC::RESPONSE_PKG *pkg)
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Response_Type::RESPONSE_DEVICE_LIST))
        return;

    auto *responsePkg = static_cast<IOTV_SC::RESPONSE_DEV_LIST_PKG*>(pkg);

    for (const auto &dev : responsePkg->devs)
    {
        auto result = _devices.emplace(dev.name, dev);
        if (result.second)
        {
            connect(&result.first->second, &Device::signalQueryRead, this, &Client::slotQueryRead);
            connect(&result.first->second, &Device::signalQueryState, this, &Client::slotQueryState);
        }
        else
        {
            Device &oldDev = result.first->second;
            if (oldDev != Device(dev))
                oldDev.update(dev);

//            QString str = QString(Q_FUNC_INFO) + " " + dev.name + " can't create new device";
//            Log::write(str, Log::Write_Flag::FILE_STDOUT);
        }
    }
}

void Client::response_STATE(IOTV_SC::RESPONSE_PKG *pkg)
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Response_Type::RESPONSE_STATE))
        return;

    auto *responsePkg = static_cast<IOTV_SC::RESPONSE_STATE_PKG*>(pkg);

    if (!_devices.contains(responsePkg->name))
    {
        QString str = QString(Q_FUNC_INFO) + " " + responsePkg->name + " not found in device list!";
        Log::write(str, Log::Write_Flag::FILE_STDOUT);
        return;
    }

    _devices[responsePkg->name].setState(responsePkg->state);
}

void Client::response_READ(IOTV_SC::RESPONSE_PKG *pkg)
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Response_Type::RESPONSE_READ))
        return;

    auto *responsePkg = static_cast<IOTV_SC::RESPONSE_READ_PKG*>(pkg);

    if (!_devices.contains(responsePkg->name))
    {
        QString str = QString(Q_FUNC_INFO) + " " + responsePkg->name + " not found in device list!";
        Log::write(str, Log::Write_Flag::FILE_STDOUT);
        return;
    }

    _devices[responsePkg->name].setData(responsePkg->channelNumber, responsePkg->data);
}

void Client::response_WRITE(IOTV_SC::RESPONSE_PKG *pkg) const
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Response_Type::RESPONSE_WRITE))
        return;

    //Нет никакой реакции на ответ о записи
}

void Client::write(const QByteArray &data)
{
    if (data.isEmpty())
        return;

    Log::write("Client transmit to server" + _socket.peerAddress().toString() + ':'
               + QString::number(_socket.peerPort())
               + " -> " + data.toHex(':'), Log::Write_Flag::FILE_STDOUT);
    _socket.write(data);
}

void Client::slotConnected()
{
    Log::write("Connected to " +
               _socket.peerAddress().toString() +
               ':' +
               QString::number(_socket.peerPort()),
               Log::Write_Flag::FILE_STDOUT);

    write(IOTV_SC::Client_TX::query_Device_List());
}

void Client::slotDisconnected()
{
    Log::write("Disconnected from " +
               _socket.peerAddress().toString() +
               ':' +
               QString::number(_socket.peerPort()),
               Log::Write_Flag::FILE_STDOUT);
}

void Client::slotReciveData()
{
    _recivedBuff += _socket.readAll();

    Log::write("Data recive from " +
               _socket.peerAddress().toString() +
               ':' +
               QString::number(_socket.peerPort()) +
               " <- " +
               _recivedBuff.toHex(':'),
               Log::Write_Flag::FILE_STDOUT);

    IOTV_SC::RESPONSE_PKG *pkg;
    while ((pkg = IOTV_SC::Client_RX::accumPacket(_recivedBuff)) != nullptr)
    {
        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_INCOMPLETE)
        {
            delete pkg;
            break;
        }

        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_ERROR)
        {
            if (_recivedBuff.size() > 0)
            {
                Log::write("WARRNING: received data from " +
                           _socket.peerName() +
                           _socket.peerAddress().toString() +
                           ':' +
                           QString::number(_socket.peerPort()) +
                           "UNKNOW: " +
                           _recivedBuff.toHex(':'),
                           Log::Write_Flag::FILE_STDOUT);
                _recivedBuff.clear();
            }
            delete pkg;
            break;
        }

        if (pkg->type == IOTV_SC::Response_Type::RESPONSE_DEVICE_LIST)
            response_DEV_LIST(pkg);
        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_STATE)
            response_STATE(pkg);
        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_READ)
            response_READ(pkg);
        else if (pkg->type == IOTV_SC::Response_Type::RESPONSE_WRITE)
            response_WRITE(pkg);
        else
        {
            //иных вариантов быть не должно!
            Log::write(QString(Q_FUNC_INFO) +
                       "Unknow pkg.type = " +
                       QString::number(int(pkg->type)),
                       Log::Write_Flag::FILE_STDERR);
            exit(-1);
        }
        delete pkg;
    }
}

void Client::slotQueryRead()
{
    Device *dev = qobject_cast<Device*>(sender());

    if ( (dev == nullptr) || !dev->isOnline())
        return;

    QByteArray data;
    for (uint8_t i = 0; i < dev->getReadChannelLength(); i++)
        data += IOTV_SC::Client_TX::query_READ(dev->getName(), i);

    write(data);
}

void Client::slotQueryState()
{
    Device *dev = qobject_cast<Device*>(sender());

    if (dev == nullptr)
        return;

    QByteArray data;
    for (uint8_t i = 0; i < dev->getReadChannelLength(); i++)
        data += IOTV_SC::Client_TX::query_STATE(dev->getName());

    write(data);
}

void Client::slotQueryDevList()
{
    write(IOTV_SC::Client_TX::query_Device_List());
}

void Client::slotError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
}
