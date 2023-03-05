#include "iotv_client.h"

IOTV_Client::IOTV_Client(QTcpSocket *socket, const std::unordered_map<IOTV_Host* , QThread*> &hosts, QObject *parent) : QObject(parent),
     _socket(socket), _hosts(hosts),
    _expectedDataSize(0)
{
    _socket->setParent(this);
    _silenceTimer.setParent(this);

    connect(&_silenceTimer, &QTimer::timeout, _socket, &QTcpSocket::disconnectFromHost, Qt::QueuedConnection);
    connect(_socket, &QTcpSocket::readyRead, this, &IOTV_Client::slotReadData, Qt::QueuedConnection);
    connect(_socket, &QTcpSocket::disconnected, this, &IOTV_Client::slotDisconnected, Qt::QueuedConnection);

    _silenceTimer.setInterval(_silenceInterval);
    _silenceTimer.start();
}

IOTV_Client::~IOTV_Client()
{
}

const QTcpSocket *IOTV_Client::socket() const
{
    return _socket;
}

void IOTV_Client::queryIdentification()
{
    char outData[BUFSIZ];

    if (_hosts.size() == 0)
    {
        auto size = responseIdentificationData(outData, BUFSIZ, NULL);
        write({outData, static_cast<int>(size)}, size);
        return;
    }

    for (const auto &host : _hosts)
    {
        struct IOTV_Server_embedded *iot = host.first->convert();
        auto size = responseIdentificationData(outData, BUFSIZ, iot);

        write({outData, static_cast<int>(size)}, size);
        clearIOTV_Server(iot);
    }
}

void IOTV_Client::queryState(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->state != NULL);

    auto it = std::ranges::find_if(_hosts, [&](const auto &iotv_host)
    {
        return iotv_host.first->getName() == QByteArray{header->state->name, header->state->nameSize};
    });

    if (it == _hosts.end())
        return;

    //!!! Для чего создавать полноценный iot, если нужно только имя и состояние?
    auto iot = it->first->convert();
    uint64_t size;
    char outData[BUFSIZ];

    size = responseStateData(outData, BUFSIZ, iot);

    write({outData, static_cast<int>(size)}, size);
    clearIOTV_Server(iot);
}

void IOTV_Client::queryRead(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->readWrite != NULL);

    auto it = std::ranges::find_if(_hosts, [&](const auto &iotv_host)
    {
        return iotv_host.first->getName() == QByteArray{header->readWrite->name, header->readWrite->nameSize};
    });

    if (it != _hosts.end())
    {
        auto iot = it->first->convert();

        uint64_t size;
        char outData[BUFSIZ];

        size = responseReadData(outData, BUFSIZ, iot, header);

        write({outData, static_cast<int>(size)}, size);

        clearIOTV_Server(iot);
    }
}

void IOTV_Client::queryWrite(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->readWrite != NULL);

    auto it = std::ranges::find_if(_hosts, [&](const auto &iotv_host)
    {
        return iotv_host.first->getName() == QByteArray{header->readWrite->name, header->readWrite->nameSize};
    });

    if (it != _hosts.end() && it->first->state() != State::State_STATE_OFFLINE)
    {
        auto iot = it->first->convert();

        uint64_t size;
        char outData[BUFSIZ];

        size = responseWriteData(outData, BUFSIZ, iot, header);
        // Ответ клиенту о записи
        write({outData, static_cast<int>(size)}, size);

        // !!! Послать данные на устройство напрямую нельзя - разные потоки
        emit it->first->signalQueryWrite(header->readWrite->channelNumber, {header->readWrite->data, static_cast<int>(header->readWrite->dataSize)});

        clearIOTV_Server(iot);
    }
}

void IOTV_Client::queryPingPoing()
{
    uint64_t size;
    char outData[BUFSIZ];

    size = responsePingData(outData, BUFSIZ);

    write({outData, static_cast<int>(size)}, size);
}

void IOTV_Client::write(const QByteArray &data, qint64 size) const
{
    Log::write("Server transmit to client " + _socket->peerAddress().toString() + ":"
               + QString::number(_socket->peerPort())
               + " -> " + data.toHex(':'), Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);
    _socket->write(data.data(), size);
}

void IOTV_Client::slotDisconnected()
{
    emit signalDisconnected();
}

void IOTV_Client::slotReadData()
{
    _silenceTimer.start();
    recivedBuff += _socket->readAll();

    Log::write("Server recive from client " + _socket->peerAddress().toString() + ":"
               + QString::number(socket()->peerPort())
               + " <- " + recivedBuff.toHex(':'), Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

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
            Log::write("Ответ от клиента не предусмотрен!",
                       Log::Write_Flag::FILE_STDOUT,
                       ServerLog::DEFAULT_LOG_FILENAME);
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
}

bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs)
{
    return lhs._socket == rhs._socket;
}
