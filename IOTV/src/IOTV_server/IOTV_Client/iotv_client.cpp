#include "iotv_client.h"

#include <sstream>
#include <fstream>
#include <thread>

IOTV_Client::IOTV_Client(QTcpSocket *socket, const std::unordered_map<IOTV_Host* , QThread*> &hosts, QObject *parent) : QObject(parent),
    _socket(socket),
    _hosts(hosts),
    _expectedDataSize(0),
    _logDataQueueTimer(nullptr),
    _my_pool(new thread_pool::ThreadPool(std::thread::hardware_concurrency()))
{
    _socket->setParent(this);
    _silenceTimer.setParent(this);
    _logDataQueueTimer.setParent(this);

    connect(&_silenceTimer, &QTimer::timeout, _socket, &QTcpSocket::disconnectFromHost);
    connect(_socket, &QTcpSocket::readyRead, this, &IOTV_Client::slotReadData);
    connect(_socket, &QTcpSocket::disconnected, this, &IOTV_Client::slotDisconnected);

    connect(&_logDataQueueTimer, &QTimer::timeout, this, &IOTV_Client::slotLogDataQueueTimerOut);
    _logDataQueueTimer.start(_LOGDATAQUEUETIMERINTERVAL);

    connect(this, &IOTV_Client::signalFetchEventActionDataFromServer, this, &IOTV_Client::slotFetchEventActionDataFromServer, Qt::QueuedConnection);

    connect(this, &IOTV_Client::signalUpdateHosts, this, &IOTV_Client::processQueryIdentification, Qt::QueuedConnection);
    connect(this, &IOTV_Client::signalClearAndUpdateHosts, this, &IOTV_Client::processQueryIdentification_2, Qt::QueuedConnection);

    _silenceTimer.start(_SILENCEINTERVAL);
}

IOTV_Client::~IOTV_Client()
{
       // qDebug() << "client destruct";
    // Закрыть все потоки, если такие имеются
    for (auto &el : _hosts)
    {
        for (int i = 0; i < el.first->getReadChannelLength(); ++i)
            emit el.first->signalRemoveStreamRead(i, this);

        //        for (int i = 0; i < el.first->getWriteChannelLength(); ++i)
        //            el.first->removeStreamWrite(i, this);

    }

    _logDataQueueTimer.stop();
    delete _my_pool;

    slotLogDataQueueTimerOut();
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
        auto size = responseIdentificationData(outData, BUFSIZ, NULL, 0);
        write({outData, static_cast<int>(size)}, size);
        return;
    }

    for (const auto &host : _hosts)
    {
        struct IOTV_Server_embedded *iot = host.first->convert();

        auto size = responseIdentificationData(outData, BUFSIZ, iot, 0);

        write({outData, static_cast<int>(size)}, size);
        clearIOTV_Server(iot);
    }
}

void IOTV_Client::processQueryIdentification_2()
{
    // В начале удаляем все устройства
    char outData[BUFSIZ];
    auto size = responseIdentificationData(outData, BUFSIZ, NULL, 0);
    write({outData, static_cast<int>(size)}, size);

    // Потом обновляем весь список
    processQueryIdentification();
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
    if (pkg == nullptr)
        return;

    auto it = std::find_if (_hosts.begin(), _hosts.end(), [&](const auto &iotv_host)
                           {
                               QString name = iotv_host.first->getName();
                               QString compareName(QByteArray{pkg->name, pkg->nameSize});

                               return name == compareName;
                               //                               const struct Read_Write *pkg = static_cast<const struct Read_Write *>(header->pkg);
                               //                               return iotv_host.first->getName() == QByteArray{pkg->name, pkg->nameSize};
                           });

    if (it == _hosts.end())
        return;

    IOTV_Host *host = it->first;

    ///!!!
    if (pkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
    {
        emit host->signalAddStreamRead(pkg->channelNumber, this);
        disconnect(host, &IOTV_Host::signalStreamRead, this, &IOTV_Client::slotStreamRead);
        connect(host, &IOTV_Host::signalStreamRead, this, &IOTV_Client::slotStreamRead, Qt::QueuedConnection);

        return;
    }
    else if (pkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
    {
        emit host->signalRemoveStreamRead(pkg->channelNumber, this);
        // связать поток клиента и поток хоста через сигналы - слоты!
        disconnect(host, &IOTV_Host::signalStreamRead, this, &IOTV_Client::slotStreamRead);

        return;
    }
    ///!!!

    auto iot = host->convert();

    char outData[BUFSIZ];
    responseReadData(outData, BUFSIZ, iot, header, &IOTV_Client::writeFunc, _socket);

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

//        qDebug() << this->thread() << it->first->thread() << this->parent();
//        it->first->write(pkg->channelNumber, {pkg->data, static_cast<int>(pkg->dataSize)});
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

    if (pkg->type == Tech_TYPE_EV_AC)
    {
        if (pkg->dataSize == 0)
            emit signalQueryEventActionData();
        else
        {
            QByteArray data(reinterpret_cast<const char *>(pkg->data), pkg->dataSize);
            emit signalFetchEventActionData(data);
        }
    }
}

void IOTV_Client::processQueryLogData(Header *header, std::atomic_int &run)
{
    auto start = std::chrono::system_clock::now();

    if (header == NULL || header->pkg == NULL)
        return;

    struct Log_Data *pkg = static_cast<struct Log_Data *>(header->pkg);
    if (pkg == nullptr)
    {
        std::lock_guard lg(_logDataQueueMutex);
        _logDataQueue.emplace(header, std::vector<char>());
        Log::write(QString(Q_FUNC_INFO) + " ошибка преобразования данных!", Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    QString compareName(QByteArray{pkg->name, pkg->nameSize});
    auto it = std::find_if (_hosts.begin(), _hosts.end(), [&](const auto &iotv_host){
        QString name = iotv_host.first->getName();
        return name == compareName;
    });

    if (it == _hosts.end())
    {
        std::lock_guard lg(_logDataQueueMutex);
        _logDataQueue.emplace(header, std::vector<char>());
        Log::write(QString(Q_FUNC_INFO) + " запрошенное устройство " + compareName + " не существует ", Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    auto host = it->first;

    time_t time = pkg->startInterval / 1000;
    std::tm *tm_ptr = localtime(&time);
    QDate startDate(tm_ptr->tm_year + 1900, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);

    QString fileName = host->logName(startDate, pkg->channelNumber);

    int64_t lastTime = 0, logLine = 0;
    std::string valueStr, line;
    char rw, tmp;
    uint64_t ms;
    int ch, year, month, day, hour, minut, second;
    std::tm tm{};

    std::ifstream file(fileName.toStdString());

    if (!file.is_open())
    {
        Log::write(QString(Q_FUNC_INFO) + " не удалось открыть файл " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
        std::lock_guard lg(_logDataQueueMutex);
        _logDataQueue.emplace(header, std::vector<char>());
        return;
    }

    std::vector<char> byteArray;
    byteArray.reserve(BUFSIZ * 1000); //!!!
    while(!file.eof() && run == thread_pool::ThreadPool::Thread_State::RUN)
    {
        ++logLine;

        if (file.eof() || file.fail())
        {
            Log::write(QString(Q_FUNC_INFO) + " ошибка данных лог файла " + fileName + " в строке " + QString::number(logLine), Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
            break;
        }

        file >> year >> tmp >> month >> tmp >> day
            >> hour >> tmp >> minut >> tmp >> second >> tmp >> ms >> tmp
            >> rw >> tmp >> ch >> tmp >> valueStr;

        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minut;
        tm.tm_sec = second;

        ms = (uint64_t)mktime(&tm) * 1000 + ms;

        // Фильтр по интервалу даты/времени
        if (ms < pkg->startInterval)
            continue;
        if (ms > pkg->endInterval)
            break;

        // Фильтр по интервалу между записями в лог файле
        if ((ms - lastTime) < pkg->interval)
            continue;

        // Фильтр номера канала
        if (pkg->channelNumber != ch)
            continue;

        // Фильтр напрвавления чтение/запись
        if ((pkg->flags == LOG_DATA_R && rw != 'R') || (pkg->flags == LOG_DATA_W && rw != 'W'))
            continue;

        lastTime = ms;

        std::string str = std::to_string(ms) + " " + valueStr + "\n";
        size_t size = str.size();
        for (size_t i = 0; i < size; i++)
            byteArray.push_back(str[i]);
    }

    if (run == thread_pool::ThreadPool::Thread_State::TERMINATE)
    {
        clearHeader(header);
        Log::write("_logDataQueue thread terminate", Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    std::lock_guard lg(_logDataQueueMutex);
    _logDataQueue.emplace(header, std::move(byteArray));

    Log::write("_logDataQueue - " + QString::number(pkg->channelNumber) + QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count()),
                Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);

    /*

    Интервал может может быть и не за одни сутки, тогда нужно парсить все лог файлы, попадающие под интервал
    second = pkg->endInterval / 1000;
    tm = localtime(&second);
    QDate endDate(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    ...
    */
}

void IOTV_Client::write(const QByteArray &data, qint64 size) const
{
    //    Log::write("Server transmit to client " + _socket->peerAddress().toString() + ":"
    //                   + QString::number(_socket->peerPort())
    //                   + " -> " + data.toHex(':'), Log::Write_Flag::FILE_STDOUT,
    //               ServerLog::DEFAULT_LOG_FILENAME);
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
       qDebug() << "client disconnect";
    emit signalDisconnected();
}

void IOTV_Client::slotReadData()
{
    _recivedBuff += _socket->readAll();

    //!!! Определится с максимальным размером буфера
    if (_recivedBuff.size() >= BUFSIZ * 10)
    {
        Log::write("slotDataResived CLIENT переполнение буфера!",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        _recivedBuff.clear();
        _socket->abort();
        return;
    }

    //    Log::write("Server recive from client " + _socket->peerAddress().toString() + ":"
    //                   + QString::number(socket()->peerPort())
    //                   + " <- " + _recivedBuff.toHex(':'), Log::Write_Flag::FILE_STDOUT,
    //               ServerLog::DEFAULT_LOG_FILENAME);

    bool error = false;
    uint64_t cutDataSize = 0;

    while (_recivedBuff.size() > 0)
    {
        struct Header* header = createPkgs(reinterpret_cast<uint8_t*>(_recivedBuff.data()), _recivedBuff.size(), &error, &_expectedDataSize, &cutDataSize);

        if (error == true)
        {
            _recivedBuff.clear();
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
            _silenceTimer.start();

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
            else if (header->assignment == HEADER_ASSIGNMENT_LOG_DATA)
            {
                _my_pool->push({thread_pool::TaskType::Execute,
                               [this, header](std::vector<thread_pool::Param> const&)
                               {
                                    this->processQueryLogData(header, std::ref(_my_pool->_run));
                               },
                               {}
                });
                _recivedBuff = _recivedBuff.mid(cutDataSize);
                continue;
            }
        }

        _recivedBuff = _recivedBuff.mid(cutDataSize);
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
        .dataSize = static_cast<uint64_t>(data.size()),
        .nameSize = iot->nameSize,
        .channelNumber = channel,
        .flags = ReadWrite_FLAGS_OPEN_STREAM,
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

void IOTV_Client::slotLogDataQueueTimerOut()
{
    std::lock_guard lg(_logDataQueueMutex);
    while (!_logDataQueue.empty())
    {
        auto pair = _logDataQueue.front();
        _logDataQueue.pop();

        auto header = pair.first;
        if (header == NULL || header->pkg == NULL)
        {
            clearHeader(header);
            continue;
        }

        const struct Log_Data *pkg = static_cast<const struct Log_Data *>(header->pkg);
        if (pkg == nullptr)
        {
            clearHeader(header);
            continue;
        }

        char outData[BUFSIZ];
        responseLogData(pair.second.data(), pair.second.size(), outData, sizeof(outData), pkg, &IOTV_Client::writeFunc, _socket);

        clearHeader(header);
    }
}

bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs)
{
    return lhs._socket == rhs._socket;
}
