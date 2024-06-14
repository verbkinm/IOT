#include "iotv_client.h"

#include <sstream>
#include <fstream>
#include <thread>

IOTV_Client::IOTV_Client(QTcpSocket *socket, QObject *parent) : QObject(parent),
    _socket(socket),
    _expectedDataSize(0),
    _logDataQueueTimer(nullptr),
    _my_pool(nullptr)//new thread_pool::ThreadPool(std::thread::hardware_concurrency()))
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

    connect(this, &IOTV_Client::signalUpdateHosts, this, &IOTV_Client::slotUpdateHosts, Qt::QueuedConnection);

    connect(this, &IOTV_Client::signalServerToClientQueryIdentification, this, &IOTV_Client::slotServerToClientQueryIdentification, Qt::QueuedConnection);
    connect(this, &IOTV_Client::signalServerToClientQueryRead, this, &IOTV_Client::slotServerToClientQueryRead, Qt::QueuedConnection);
    connect(this, &IOTV_Client::signalServerToClientQueryWrite, this, &IOTV_Client::slotServerToClientQueryWrite, Qt::QueuedConnection);
    connect(this, &IOTV_Client::signalServerToClientQueryState, this, &IOTV_Client::slotServerToClientQueryState, Qt::QueuedConnection);
    connect(this, &IOTV_Client::signalServerToClientQueryLogData, this, &IOTV_Client::slotServerToClientQueryLogData, Qt::QueuedConnection);

    _silenceTimer.start(_SILENCEINTERVAL);
}

IOTV_Client::~IOTV_Client()
{
    // qDebug() << "client destruct";
    // Закрыть все потоки, если такие имеются
//    for (auto &el : _hosts)
//    {
//        for (int i = 0; i < el.first->getReadChannelLength(); ++i)
//            emit el.first->signalRemoveStreamRead(i, this);

//        //        for (int i = 0; i < el.first->getWriteChannelLength(); ++i)
//        //            el.first->removeStreamWrite(i, this);

//    }

    _logDataQueueTimer.stop();
//    delete _my_pool;

    slotLogDataQueueTimerOut();
}

const QTcpSocket *IOTV_Client::socket() const
{
    return _socket;
}

void IOTV_Client::processQueryLogData(RAII_Header raii_header, QString fileName, bool hostError, std::atomic_int &run)
{
    auto start = std::chrono::system_clock::now();

    if (raii_header.header() == NULL)
        return;

    struct Log_Data *pkg = static_cast<struct Log_Data *>(raii_header.header()->pkg);
    if (pkg == nullptr)
    {
        std::lock_guard lg(_logDataQueueMutex);
        _logDataQueue.emplace(raii_header, std::vector<char>());
        Log::write(QString(Q_FUNC_INFO) + " ошибка преобразования данных!", Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    QString compareName(QByteArray{pkg->name, pkg->nameSize});
    if (hostError)
    {
        std::lock_guard lg(_logDataQueueMutex);
        _logDataQueue.emplace(raii_header, std::vector<char>());
        Log::write(QString(Q_FUNC_INFO) + " запрошенное устройство " + compareName + " не существует ", Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

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
        _logDataQueue.emplace(raii_header, std::vector<char>());
        return;
    }

    std::vector<char> byteArray;
    byteArray.reserve(BUFSIZ * 1000); //!!!
    while(!file.eof())// && run == thread_pool::ThreadPool::Thread_State::RUN)
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

//    if (run == thread_pool::ThreadPool::Thread_State::TERMINATE)
//    {
//        Log::write("_logDataQueue thread terminate", Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
//        return;
//    }

    std::lock_guard lg(_logDataQueueMutex);
    _logDataQueue.emplace(raii_header, std::move(byteArray));

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
        header_t *header = createPkgs(reinterpret_cast<uint8_t*>(_recivedBuff.data()), _recivedBuff.size(), &error, &_expectedDataSize, &cutDataSize);
        RAII_Header raii_header(header);
        clearHeader(header);

        if (error == true)
        {
            _recivedBuff.clear();
            _expectedDataSize = 0;
            cutDataSize = 0;
            break;
        }

        // Пакет не ещё полный
        if (_expectedDataSize > 0)
            break;

        if (raii_header.header()->type == HEADER_TYPE_RESPONSE)
        {
            // На данный момент от клиента не должно приходить ответов
            Log::write("Ответ от клиента не предусмотрен!",
                       Log::Write_Flag::FILE_STDOUT,
                       ServerLog::DEFAULT_LOG_FILENAME);
        }
        else if (raii_header.header()->type == HEADER_TYPE_REQUEST)
        {
            _silenceTimer.start();

            if (raii_header.header()->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
                emit signalClientToServerQueryIdentification();
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_READ)
                emit signalClientToServerQueryRead(raii_header);
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_WRITE)
                emit signalClientToServerQueryWrite(raii_header);
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_PING_PONG)
            {
                uint64_t size;
                char outData[BUFSIZ];

                size = responsePingData(outData, BUFSIZ);

                write({outData, static_cast<int>(size)}, size);
            }
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_STATE)
                emit signalClientToServerQueryState(raii_header);
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_TECH)
                emit signalClientToServerQueryTech(raii_header);
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_LOG_DATA)
            {
                emit signalClientToServerLogData(raii_header);
//                 _my_pool->push({thread_pool::TaskType::Execute,
//                                [this, header](std::vector<thread_pool::Param> const&)
//                                {
//                                     this->processQueryLogData(header, std::ref(_my_pool->_run));
//                                },
//                                {}
//                 });
//                 _recivedBuff = _recivedBuff.mid(cutDataSize);
//                 continue;
            }
        }

        _recivedBuff = _recivedBuff.mid(cutDataSize);
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

    header_t header = {
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
        .dataSize = static_cast<uint32_t>(data.size()),
        .nameSize = iot->nameSize,
        .channelNumber = channel,
        .flags = ReadWrite_FLAGS_OPEN_STREAM,
        .name = iot->name,
        .data = data.data()
    };

    header_t header = {
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

    clear_iotv_obj(iot);
}

void IOTV_Client::slotLogDataQueueTimerOut()
{
    std::lock_guard lg(_logDataQueueMutex);
    while (!_logDataQueue.empty())
    {
        auto pair = _logDataQueue.front();
        _logDataQueue.pop();

        auto raii_header = pair.first;
        if (raii_header.header() == NULL || raii_header.header()->pkg == NULL)
            continue;

        const struct Log_Data *pkg = static_cast<const struct Log_Data *>(raii_header.header()->pkg);
        if (pkg == nullptr)
            continue;

        char outData[BUFSIZ];
        responseLogData(pair.second.data(), pair.second.size(), outData, sizeof(outData), pkg, &IOTV_Client::writeFunc, _socket);
    }
}

void IOTV_Client::slotUpdateHosts(QByteArray data)
{
    // В начале удаляем все устройства
    char outData[BUFSIZ];
    auto size = responseIdentificationData(outData, BUFSIZ, NULL, Identification_FLAGS_NONE);
    write({outData, static_cast<int>(size)}, size);

    // Потом обновляем весь список
    write(data, data.size());
}

void IOTV_Client::slotServerToClientQueryIdentification(QByteArray data)
{
    write(data, data.size());
}

void IOTV_Client::slotServerToClientQueryRead(RAII_Header raii_header, RAII_iot raii_iot)
{
    char outData[BUFSIZ];
    responseReadData(outData, BUFSIZ, raii_iot.iot(), raii_header.header(), &IOTV_Client::writeFunc, _socket, ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
}

void IOTV_Client::slotServerToClientQueryWrite(RAII_Header raii_header, RAII_iot raii_iot)
{
    uint64_t size;
    char outData[BUFSIZ];

    size = responseWriteData(outData, BUFSIZ, raii_iot.iot(), raii_header.header(), ReadWrite_FLAGS_NONE, HEADER_FLAGS_NONE);
    // Ответ клиенту о записи
    write({outData, static_cast<int>(size)}, size);
}

void IOTV_Client::slotServerToClientQueryState(RAII_iot raii_iot)
{
    uint64_t size;
    char outData[BUFSIZ];

    size = responseStateData(outData, BUFSIZ, raii_iot.iot());

    write({outData, static_cast<int>(size)}, size);
}

void IOTV_Client::slotServerToClientQueryLogData(RAII_Header raii_header, QString logName, bool hostError)
{
    processQueryLogData(raii_header, logName, hostError, std::ref(_my_pool->_run));
//!!!
//    _my_pool->push({thread_pool::TaskType::Execute,
//                                [&](std::vector<thread_pool::Param> const&)
//                                {
//                                     this->processQueryLogData(raii_header, logName, hostError, std::ref(_my_pool->_run));
//                                },
//                                {}
//                 });
}

bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs)
{
    return lhs._socket == rhs._socket;
}
