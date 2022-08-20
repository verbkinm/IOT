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

void IOTV_Client::query_DEV_LIST_recived(Server_RX::QUERY_PKG *pkg)
{
    if (pkg == nullptr)
        return;

    if (pkg->type != Server_RX::Query_Type::QUERY_DEVICE_LIST)
    {
        delete pkg;
        return;
    }

    auto *devListPkg = static_cast<const Server_RX::QUERY_DEV_LIST_PKG*>(pkg);

    //!!!
//    devListPkg->
}

void IOTV_Client::query_STATE_recived(Server_RX::QUERY_PKG *pkg)
{
    if (pkg == nullptr)
        return;

    if (pkg->type != Server_RX::Query_Type::QUERY_STATE)
    {
        delete pkg;
        return;
    }

    auto *statePkg = static_cast<const Server_RX::QUERY_STATE_PKG*>(pkg);

    auto it = std::ranges::find_if(_hosts, [&](const IOTV_Host &iotv_host)
    {
        return iotv_host.getName() == statePkg->name;
    });

    if (it != _hosts.end())
    {
        //!!!
    }

//    _hosts.statePkg->name
}

void IOTV_Client::query_READ_recived(Server_RX::QUERY_PKG *pkg)
{

}

void IOTV_Client::query_WRITE_recived(Server_RX::QUERY_PKG *pkg)
{

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
    recivedBuff += _socket->readAll();

    Log::write("Server recive from client " + _socket->peerAddress().toString() + ":"
               + QString::number(socket()->peerPort())
               + " <- " + recivedBuff.toHex(':'), Log::Write_Flag::FILE_STDOUT);

    Server_RX::QUERY_PKG *pkg;
    while ((pkg = Server_RX::accumPacket(recivedBuff)) != nullptr)
    {
        if (pkg->type == Server_RX::Query_Type::QUERY_INCOMPLETE)
        {
            if (pkg->type == Server_RX::Query_Type::QUERY_ERROR)
            {
                Log::write("WARRNING: received data from " +
                           _socket->peerName() +
                           _socket->peerAddress().toString() +
                           ":" +
                           QString::number(_socket->peerPort()) +
                           "UNKNOW: " + recivedBuff.toHex(':'), Log::Write_Flag::FILE_STDOUT);
            }
            delete pkg;
            break;
        }

        if (pkg->type == Server_RX::Query_Type::QUERY_DEVICE_LIST)
            query_DEV_LIST_recived(pkg);
        else if (pkg->type == Server_RX::Query_Type::QUERY_STATE)
            query_STATE_recived(pkg);
        else if (pkg->type == Server_RX::Query_Type::QUERY_READ)
            query_STATE_recived(pkg);
        else if (pkg->type == Server_RX::Query_Type::QUERY_WRITE)
            query_WRITE_recived(pkg);
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

bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs)
{
    return lhs._socket == rhs._socket;
}
