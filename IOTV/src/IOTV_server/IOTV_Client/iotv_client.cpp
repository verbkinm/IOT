#include "iotv_client.h"

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
    ///!!! закрыть все потоки, если такие имеются
    for (auto &el : _hosts)
    {
        for (int i = 0; i < el.first->getReadChannelLength(); ++i)
            el.first->removeStreamRead(i, this);

        //        for (int i = 0; i < el.first->getWriteChannelLength(); ++i)
        //            el.first->removeStreamWrite(i, this);

    }
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
    if (header == NULL || header->pkg == NULL)
        return;

    const struct Read_Write *pkg = static_cast<const struct Read_Write *>(header->pkg);
    auto it = std::find_if (_hosts.begin(), _hosts.end(), [&](const auto &iotv_host)
                           {
                               //                               const struct Read_Write *pkg = static_cast<const struct Read_Write *>(header->pkg);
                               return iotv_host.first->getName() == QByteArray{pkg->name, pkg->nameSize};
                           });

    if (it == _hosts.end())
        return;

    ///!!!
    if (pkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
    {
        if (it->first->addStreamRead(pkg->channelNumber, this))
            connect(it->first, &IOTV_Host::signalStreamRead, this, &IOTV_Client::slotStreamRead, Qt::QueuedConnection);

        return;
    }
    else if (pkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
    {
        it->first->removeStreamRead(pkg->channelNumber, this);
        disconnect(it->first, &IOTV_Host::signalStreamRead, this, &IOTV_Client::slotStreamRead);

        return;
    }
    ///!!!

    auto iot = it->first->convert();

    uint64_t size;
    char outData[BUFSIZ];

    size = responseReadData(outData, BUFSIZ, iot, header, &IOTV_Client::writeFunc, _socket);

    write({outData, static_cast<int>(size)}, size);

    clearIOTV_Server(iot);
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

uint64_t IOTV_Client::writeFunc(char *data, uint64_t size, void *obj)
{
    QTcpSocket *socket = static_cast<QTcpSocket *>(obj);

    if (socket == NULL)
        return 0;

    return socket->write(data, size);
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
            // На данный момент от клиента не должно приходить ответов
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
        .data = reinterpret_cast<uint8_t *>(data.data())
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_TECH,
        .flags = HEADER_FLAGS_NONE,
        .fragment = 1,
        .fragments = 1,
        .dataSize = techSize(&tech),
        .pkg = &tech
    };


    size = responseTech(outData, BUFSIZ, data.data(), data.size(), &header);
    write({outData, static_cast<int>(size)}, size);
}

void IOTV_Client::slotStreamRead(uint8_t channel, uint16_t fragment, uint16_t fragments, QByteArray data)
{
    IOTV_Host *host = dynamic_cast<IOTV_Host *>(sender());
    if (host == nullptr)
        return;

    auto iot = host->convert();
    iot->readChannel[channel].dataSize = data.size();
    iot->readChannel[channel].data = data.data();


    struct Read_Write read = {
        .nameSize = iot->nameSize,
        .channelNumber = channel,
        .flags = ReadWrite_FLAGS_OPEN_STREAM,
        .dataSize = static_cast<uint64_t>(data.size()),
        .name = iot->name,
        .data = data.data()
    };

    struct Header header = {
        .version = 2,
        .type = HEADER_TYPE_RESPONSE,
        .assignment = HEADER_ASSIGNMENT_READ,
        .flags = HEADER_FLAGS_NONE,
        .fragment = fragment,
        .fragments = fragments,
        .dataSize = readWriteSize(&read),
        .pkg = &read
    };

    auto outDataSize = headerSize(&header);
    if (outDataSize > BUFSIZ)
    {
        char *outData = (char *)malloc(outDataSize);
        if (outData == nullptr)
            return;

        auto size = headerToData(&header, outData, outDataSize);
        _socket->write(outData, size);

        free(outData);
    }
    else
    {
        char outData[headerSize(&header)];
        auto size = headerToData(&header, outData, BUFSIZ);
        _socket->write(outData, size);
    }

    iot->readChannel[channel].dataSize = 0;
    iot->readChannel[channel].data = 0;

    clearIOTV_Server(iot);
}

bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs)
{
    return lhs._socket == rhs._socket;
}
