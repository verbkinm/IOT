#include "iotv_client.h"
#include "event_action_parser.h"

IOTV_Client::IOTV_Client(QTcpSocket *socket, const std::unordered_map<IOTV_Host* , QThread*> &hosts, QObject *parent) : QObject(parent),
    _socket(socket),
    _hosts(hosts),
    _expectedDataSize(0)
{
    _socket->setParent(this);
    _silenceTimer.setParent(this);

    connect(&_silenceTimer, &QTimer::timeout, _socket, &QTcpSocket::disconnectFromHost, Qt::QueuedConnection);
    connect(_socket, &QTcpSocket::readyRead, this, &IOTV_Client::slotReadData, Qt::QueuedConnection);
    connect(_socket, &QTcpSocket::disconnected, this, &IOTV_Client::slotDisconnected, Qt::QueuedConnection);

    connect(this, &IOTV_Client::signalFetchEventActionDataFromServer, this, &IOTV_Client::slotFetchEventActionDataFromServer, Qt::QueuedConnection);


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

void IOTV_Client::processQueryIdentification()
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

void IOTV_Client::processQueryState(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    auto it = std::find_if (_hosts.begin(), _hosts.end(), [&](const auto &iotv_host)
                                   {
                                       const struct State *pkg = static_cast<const struct State *>(header->pkg);
                                       return iotv_host.first->getName() == QByteArray{pkg->name, pkg->nameSize};
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

void IOTV_Client::processQueryRead(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    auto it = std::find_if (_hosts.begin(), _hosts.end(), [&](const auto &iotv_host)
                                   {
                                       const struct Read_Write *pkg = static_cast<const struct Read_Write *>(header->pkg);
                                       return iotv_host.first->getName() == QByteArray{pkg->name, pkg->nameSize};
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

void IOTV_Client::processQueryWrite(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct Read_Write *pkg = static_cast<const struct Read_Write *>(header->pkg);

    auto it = std::find_if (_hosts.begin(), _hosts.end(), [&](const auto &iotv_host)
                                   {
                                       return iotv_host.first->getName() == QByteArray{pkg->name, pkg->nameSize};
                                   });

    if (it != _hosts.end() && it->first->state() != State_STATE_OFFLINE)
    {
        auto iot = it->first->convert();

        uint64_t size;
        char outData[BUFSIZ];

        size = responseWriteData(outData, BUFSIZ, iot, header);
        // Ответ клиенту о записи
        write({outData, static_cast<int>(size)}, size);

        // Послать данные на устройство напрямую нельзя - разные потоки
        emit it->first->signalQueryWrite(pkg->channelNumber, {pkg->data, static_cast<int>(pkg->dataSize)});

        clearIOTV_Server(iot);
    }
}

void IOTV_Client::processQueryPingPoing()
{
    uint64_t size;
    char outData[BUFSIZ];

    size = responsePingData(outData, BUFSIZ);

    write({outData, static_cast<int>(size)}, size);
}

void IOTV_Client::processQueryTech(const Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct Tech *pkg = static_cast<const struct Tech *>(header->pkg);

    //    uint64_t size;
    //    char outData[BUFSIZ];

    if (pkg->type == Tech_TYPE_EV_AC)
    {
        if (pkg->dataSize == 0)
        {
            emit signalQueryEventActionData();
            //            if (_eventManager != nullptr)
            //            {
            //                std::string str = Event_Action_Parser::toData(_eventManager->worker()).toStdString();
            //                size = responseTech(outData, BUFSIZ, str.c_str(), str.size(), header);
            //                write({outData, static_cast<int>(size)}, size);
            //            }
        }
        else
        {
            QByteArray data(reinterpret_cast<const char *>(pkg->data), pkg->dataSize);
            emit signalFetchEventActionData(data);
        }
    }
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

        if (header->type == HEADER_TYPE_RESPONSE)
        {
            // На данный момент от клиент не должно приходить ответов
            Log::write("Ответ от клиента не предусмотрен!",
                       Log::Write_Flag::FILE_STDOUT,
                       ServerLog::DEFAULT_LOG_FILENAME);
        }
        else if (header->type == HEADER_TYPE_REQUEST)
        {
            if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
                processQueryIdentification();
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
                processQueryRead(header);
            else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
                processQueryWrite(header);
            else if (header->assignment == HEADER_ASSIGNMENT_PING_PONG)
                processQueryPingPoing();
            else if (header->assignment == HEADER_ASSIGNMENT_STATE)
                processQueryState(header);
            else if (header->assignment == HEADER_ASSIGNMENT_TECH)
                processQueryTech(header);
        }

        recivedBuff = recivedBuff.mid(cutDataSize);

        clearHeader(header);
    }
}

void IOTV_Client::slotFetchEventActionDataFromServer(QByteArray data)
{
    uint64_t size;
    char outData[BUFSIZ];

    struct Tech tech = {
        .flags = Tech_FLAGS_NONE,
        .type = Tech_TYPE_EV_AC,
        .dataSize = static_cast<uint64_t>(data.size()),
        .data = (const uint8_t*)data.data()
    };

    struct Header header = {
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_TECH,
        .flags = HEADER_FLAGS_NONE,
        .version = 2,
        .dataSize = techSize(&tech),
        .pkg = &tech
    };


    size = responseTech(outData, BUFSIZ, data.data(), data.size(), &header);
    write({outData, static_cast<int>(size)}, size);
}

bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs)
{
    return lhs._socket == rhs._socket;
}
