#include "iotv_client.h"

IOTV_Client::IOTV_Client(QTcpSocket *socket, std::list<IOTV_Host> &hosts, QObject *parent) : QObject(parent),
    _parentThread(QThread::currentThread()), _socket(socket), _hosts(hosts)
{
    _socket->setParent(this);
    connect(&_thread, &QThread::started, this, &IOTV_Client::slotNewThreadStart, Qt::QueuedConnection);
    connect(this, &IOTV_Client::signalStopThread, this, &IOTV_Client::slotThreadStop, Qt::QueuedConnection);
}

IOTV_Client::~IOTV_Client()
{
    emit signalStopThread();
    _thread.wait();
}

bool IOTV_Client::runInNewThread()
{
    if (_parentThread == &_thread)
        return false;

    this->moveToThread(&_thread);

    _thread.start();

    return _thread.isRunning();
}

const QTcpSocket *IOTV_Client::socket() const
{
    return _socket;
}

void IOTV_Client::slotDisconnected()
{
    emit signalDisconnected();
}

void IOTV_Client::slotNewThreadStart()
{
    connect(_socket, &QTcpSocket::readyRead, this, &IOTV_Client::slotReadData);
    connect(_socket, &QTcpSocket::disconnected, this, &IOTV_Client::slotDisconnected);
    connect(_socket, &QTcpSocket::disconnected, _socket, &QObject::deleteLater);
}

void IOTV_Client::slotThreadStop()
{
    if (_parentThread == nullptr)
        return;

    this->moveToThread(_parentThread);

    _thread.exit();
}

void IOTV_Client::slotReadData()
{
    QByteArray data = _socket->readAll();

    Log::write("Server recive from client " + _socket->peerAddress().toString() + ":"
               + QString::number(socket()->peerPort())
               + " <- " + data.toHex(':'), Log::Write_Flag::FILE_STDOUT);

}

bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs)
{
    return lhs._socket == rhs._socket;
}
