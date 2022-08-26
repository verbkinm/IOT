#include "client.h"

Client::Client(const QString &address, const quint16 &port, QObject *parent) : QObject{parent},
    _address{address}, _port{port}
{
    connect(&_socket, &QTcpSocket::connected, this, &Client::slotConnected);
    connect(&_socket, &QTcpSocket::disconnected, this, &Client::slotDisconnected);
    connect(&_socket, &QTcpSocket::readyRead, this, &Client::slotReadData);
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

void Client::slotConnected()
{
    Log::write("Connected to " +
               _socket.peerAddress().toString() +
               ":" +
               QString::number(_socket.peerPort()),
               Log::Write_Flag::FILE_STDOUT);

    _socket.write(IOTV_SC::Client_TX::query_Device_List());
}

void Client::slotDisconnected()
{
    Log::write("Disconnected from " +
               _socket.peerAddress().toString() +
               ":" +
               QString::number(_socket.peerPort()),
               Log::Write_Flag::FILE_STDOUT);
}

void Client::slotReadData()
{
    QByteArray data = _socket.readAll();

    Log::write("Data recive from " +
               _socket.peerAddress().toString() +
               ":" +
               QString::number(_socket.peerPort()) +
               " <- " +
               data.toHex(':'),
               Log::Write_Flag::FILE_STDOUT);
}

void Client::slotError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
}
