#include "iotv_client.h"

IOTV_Client::IOTV_Client(QTcpSocket *socket, std::list<IOTV_Host> &hosts, QObject *parent) : QObject(parent),
    _parentThread(QThread::currentThread()), _socket(socket), _hosts(hosts),
    _silenceInterval(60000),
    _expectedDataSize(0)
{
    _socket->setParent(this);
    _silenceTimer.setParent(this);

    connect(&_thread, &QThread::started, this, &IOTV_Client::slotNewThreadStart, Qt::QueuedConnection);
    connect(this, &IOTV_Client::signalStopThread, this, &IOTV_Client::slotThreadStop, Qt::QueuedConnection);

    connect(&_silenceTimer, &QTimer::timeout, _socket, &QTcpSocket::disconnectFromHost);

    _silenceTimer.setInterval(_silenceInterval);
    _silenceTimer.start();
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

void IOTV_Client::queryIdentification()
{
    char outData[BUFSIZ];
    for (const auto &host : _hosts)
    {
        struct IOTV_Server_embedded *iot = host.convert();
        auto size = responseIdentificationData(outData, BUFSIZ, iot);

        write({outData, static_cast<qsizetype>(size)}, size);

        clearIOTV_Server(iot);
    }
}

void IOTV_Client::queryState(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->state != NULL);

    auto it = std::ranges::find_if(_hosts, [&](const IOTV_Host &iotv_host)
    {
        return strcmp(iotv_host.getName().toStdString().c_str(), header->state->name) == 0;
    });

    if (it != _hosts.end())
    {
        //        struct IOTV_Server_embedded iot = {
        //            .id = 0,
        //                    .name = header->state->name,
        //                    .description = NULL,
        //                    .numberReadChannel = 0,
        //                    .readChannel = NULL,
        //                    .readChannelType = NULL,
        //                    .numberWriteChannel = 0,
        //                    .writeChannelType = NULL,
        //                    .state = it->isOnline()
        //        };

        //!!! Для чего создавать полноценный iot, если нужно только имя и состояние?
        auto iot = it->convert();
        uint64_t size;
        char outData[BUFSIZ];

        size = responseStateData(outData, BUFSIZ, iot);

        write({outData, static_cast<qsizetype>(size)}, size);
        clearIOTV_Server(iot);
    }
}

void IOTV_Client::queryRead(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->readWrite != NULL);

    auto it = std::ranges::find_if(_hosts, [&](const IOTV_Host &iotv_host)
    {
        return memcmp(iotv_host.getName().toStdString().c_str(), header->readWrite->name, iotv_host.getName().toStdString().size());
//        return strcmp(iotv_host.getName().toStdString().c_str(), header->readWrite->name) == 0;
    });

    if (it != _hosts.end())
    {
        auto iot = it->convert();

        uint64_t size;
        char outData[BUFSIZ];

        size = responseReadData(outData, BUFSIZ, iot, header);

        write({outData, static_cast<qsizetype>(size)});

        clearIOTV_Server(iot);
    }
}

void IOTV_Client::queryWrite(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->readWrite != NULL);

    auto it = std::ranges::find_if(_hosts, [&](const IOTV_Host &iotv_host)
    {
        return strcmp(iotv_host.getName().toStdString().c_str(), header->state->name) == 0;
    });

    if (it != _hosts.end())
    {
        auto iot = it->convert();

        uint64_t size;
        char outData[BUFSIZ];

        size = responseWriteData(outData, BUFSIZ, iot, header);

        QByteArray writeData(header->readWrite->data, header->readWrite->dataSize);
        // Послать данные на устройство
        it->write(header->readWrite->channelNumber, writeData);

        QByteArray rawData(outData, size);
//        write(rawData, size);

        emit it->signalQueryWrite(header->readWrite->channelNumber, rawData);

        clearIOTV_Server(iot);
    }
}

void IOTV_Client::queryPingPoing()
{
    uint64_t size;
    char outData[BUFSIZ];

    size = responsePingData(outData, BUFSIZ);

    write({outData, static_cast<qsizetype>(size)});
}

void IOTV_Client::write(const QByteArray &data, qint64 size) const
{
    Log::write("Server transmit to client " + _socket->peerAddress().toString() + ":"
               + QString::number(_socket->peerPort())
               + " -> " + data.toHex(':'), Log::Write_Flag::FILE_STDOUT);
    _socket->write(data.data(), size);
}

void IOTV_Client::slotDisconnected()
{
    emit signalDisconnected();
}

void IOTV_Client::slotNewThreadStart()
{
    connect(_socket, &QTcpSocket::readyRead, this, &IOTV_Client::slotReadData);
    connect(_socket, &QTcpSocket::disconnected, this, &IOTV_Client::slotDisconnected);
}

void IOTV_Client::slotThreadStop()
{
    if (_socket != nullptr)
        delete _socket;

    if (_parentThread == nullptr)
        return;

    this->moveToThread(_parentThread);

    _thread.exit();
}

void IOTV_Client::slotReadData()
{
    _silenceTimer.start();
    recivedBuff += _socket->readAll();

    bool error = false;
    uint64_t cutDataSize = 0;

    while (recivedBuff.size() > 0)
    {
        struct Header* header = createPkgs(reinterpret_cast<uint8_t*>(recivedBuff.data()), recivedBuff.size(), &error, &_expectedDataSize, &cutDataSize);

        if (error == true)
        {
            recivedBuff.clear();
            _expectedDataSize = 0;
            cutDataSize = 0;
            clearHeader(header);
            break;
        }

        // Пакет не ещё полный
        if (_expectedDataSize > 0)
        {
            clearHeader(header);
            break;
        }

        if (header->type == Header::HEADER_TYPE_RESPONSE)
        {
            // На данный момент от клиент не должно приходить ответов
            Log::write("Ответ от клиента не предусмотрен!");
        }
        else if(header->type == Header::HEADER_TYPE_REQUEST)
        {
            if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
                queryIdentification();
            else if(header->assignment == Header::HEADER_ASSIGNMENT_READ)
                queryRead(header);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
                queryWrite(header);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_PING_PONG)
                queryPingPoing();
            else if(header->assignment == Header::HEADER_ASSIGNMENT_STATE)
                queryState(header);
        }

        recivedBuff = recivedBuff.mid(cutDataSize);

        clearHeader(header);
    }

    Log::write("Server recive from client " + _socket->peerAddress().toString() + ":"
               + QString::number(socket()->peerPort())
               + " <- " + recivedBuff.toHex(':'), Log::Write_Flag::FILE_STDOUT);

    //    IOTV_SC::Server_RX::QUERY_PKG *pkg;
    //    while ((pkg = IOTV_SC::Server_RX::accumPacket(recivedBuff)) != nullptr)
    //    {
    //        if (pkg->type == IOTV_SC::Query_Type::QUERY_INCOMPLETE)
    //        {
    //            delete pkg;
    //            break;
    //        }

    //        if (pkg->type == IOTV_SC::Query_Type::QUERY_ERROR)
    //        {
    //            if (recivedBuff.size() > 0)
    //            {
    //                Log::write("WARRNING: received data from " +
    //                           _socket->peerName() +
    //                           _socket->peerAddress().toString() +
    //                           ":" +
    //                           QString::number(_socket->peerPort()) +
    //                           "UNKNOW: " +
    //                           recivedBuff.toHex(':'),
    //                           Log::Write_Flag::FILE_STDOUT);
    //                recivedBuff.clear();
    //            }
    //            delete pkg;
    //            break;
    //        }

    //        if (pkg->type == IOTV_SC::Query_Type::QUERY_DEVICE_LIST)
    //            query_DEV_LIST_recived(pkg);
    //        else if (pkg->type == IOTV_SC::Query_Type::QUERY_STATE)
    //            query_STATE_recived(pkg);
    //        else if (pkg->type == IOTV_SC::Query_Type::QUERY_READ)
    //            query_READ_recived(pkg);
    //        else if (pkg->type == IOTV_SC::Query_Type::QUERY_WRITE)
    //            query_WRITE_recived(pkg);
    //        else
    //        {
    //            //иных вариантов быть не должно!
    //            Log::write(QString(Q_FUNC_INFO) +
    //                       "Unknow pkg.type = " +
    //                       QString::number(int(pkg->type)),
    //                       Log::Write_Flag::FILE_STDERR);
    //            exit(-1);
    //        }
    //        delete pkg;
    //    }
}

bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs)
{
    return lhs._socket == rhs._socket;
}
