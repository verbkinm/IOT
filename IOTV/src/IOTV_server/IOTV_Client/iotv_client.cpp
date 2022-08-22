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

void IOTV_Client::query_DEV_LIST_recived(IOTV_SC::Server_RX::QUERY_PKG *pkg) const
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Query_Type::QUERY_DEVICE_LIST))
        return;

    IOTV_SC::RESPONSE_DEV_LIST_PKG responseDevListPkg;
    responseDevListPkg.devs.reserve(_hosts.size());
    for (const auto &host : _hosts)
    {
        IOTV_SC::DEV_PKG dev;
        dev.id = host.getId();
        dev.name = host.getName();
        dev.description = host.getDescription();

        dev.readChannel.resize(host.getReadChannelLength());
        for (uint8_t i = 0; i < host.getReadChannelLength(); i++)
            dev.readChannel.at(i) = host.getReadChannelType(i);

        dev.writeChannel.resize(host.getWriteChannelLength());
        for (uint8_t i = 0; i < host.getWriteChannelLength(); i++)
            dev.writeChannel.at(i) = host.getWriteChannelType(i);

        responseDevListPkg.devs.push_back(dev);
    }

    _socket->write(IOTV_SC::Server_TX::response_DEV_LIST(responseDevListPkg));
}

void IOTV_Client::query_STATE_recived(IOTV_SC::Server_RX::QUERY_PKG *pkg) const
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Query_Type::QUERY_STATE))
        return;

    auto queryStatePkg = static_cast<IOTV_SC::Server_RX::QUERY_STATE_PKG*>(pkg);
    IOTV_SC::RESPONSE_STATE_PKG responseStatePkg;
    responseStatePkg.name = queryStatePkg->name;

    auto it = std::ranges::find_if(_hosts, [&](const IOTV_Host &iotv_host)
    {
        return iotv_host.getName() == queryStatePkg->name;
    });

    if (it != _hosts.end())
        responseStatePkg.state = it->isOnline();
    else
        responseStatePkg.state = false;

    _socket->write(IOTV_SC::Server_TX::response_STATE(responseStatePkg));
}

void IOTV_Client::query_READ_recived(IOTV_SC::Server_RX::QUERY_PKG *pkg) const
{
    if ((pkg == nullptr) || (pkg->type != IOTV_SC::Query_Type::QUERY_READ))
        return;

    auto queryReadPkg = static_cast<IOTV_SC::Server_RX::QUERY_READ_PKG*>(pkg);
    IOTV_SC::RESPONSE_READ_PKG responseReadPkg;
    responseReadPkg.name = queryReadPkg->name;
    responseReadPkg.channelNumber = queryReadPkg->channelNumber;

    auto it = std::ranges::find_if(_hosts, [&](const IOTV_Host &iotv_host)
    {
        return iotv_host.getName() == queryReadPkg->name;
    });

    if (it != _hosts.end())
        responseReadPkg.data = it->readData(responseReadPkg.channelNumber);

    _socket->write(IOTV_SC::Server_TX::response_READ(responseReadPkg));
}

void IOTV_Client::query_WRITE_recived(IOTV_SC::Server_RX::QUERY_PKG *pkg) const
{
    if (pkg == nullptr)
        return;

    if (pkg->type != IOTV_SC::Query_Type::QUERY_WRITE)
        return;

    auto queryWritePkg = static_cast<IOTV_SC::Server_RX::QUERY_WRITE_PKG*>(pkg);
    IOTV_SC::RESPONSE_WRITE_PKG responseWritePkg;
    responseWritePkg.name = queryWritePkg->name;
    responseWritePkg.channelNumber = queryWritePkg->channelNumber;

    _socket->write(IOTV_SC::Server_TX::response_WRITE(responseWritePkg));
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

    IOTV_SC::Server_RX::QUERY_PKG *pkg;
    while ((pkg = IOTV_SC::Server_RX::accumPacket(recivedBuff)) != nullptr)
    {
        if (pkg->type == IOTV_SC::Query_Type::QUERY_INCOMPLETE)
        {
            delete pkg;
            break;
        }

        if (pkg->type == IOTV_SC::Query_Type::QUERY_ERROR)
        {
            if (recivedBuff.size() > 0)
            {
                Log::write("WARRNING: received data from " +
                           _socket->peerName() +
                           _socket->peerAddress().toString() +
                           ":" +
                           QString::number(_socket->peerPort()) +
                           "UNKNOW: " +
                           recivedBuff.toHex(':'),
                           Log::Write_Flag::FILE_STDOUT);
                recivedBuff.clear();
            }
            delete pkg;
            break;
        }

        if (pkg->type == IOTV_SC::Query_Type::QUERY_DEVICE_LIST)
            query_DEV_LIST_recived(pkg);
        else if (pkg->type == IOTV_SC::Query_Type::QUERY_STATE)
            query_STATE_recived(pkg);
        else if (pkg->type == IOTV_SC::Query_Type::QUERY_READ)
            query_STATE_recived(pkg);
        else if (pkg->type == IOTV_SC::Query_Type::QUERY_WRITE)
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
