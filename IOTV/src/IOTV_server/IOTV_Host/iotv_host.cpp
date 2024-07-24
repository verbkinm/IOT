#include "iotv_host.h"
#include "qthread.h"
#include "raii_iot.h"


#include <QFileInfo>
#include <QDate>

IOTV_Host::IOTV_Host(const std::unordered_map<QString, QString> &settingsData, QObject* parent) : Base_Host(0, parent),
    _logDir(settingsData.at(hostField::logDir)),
    _settingsData(settingsData),
    _counterState(0), _counterPing(0),
    _fragIdent(BUFSIZ * 10)
{
    shareConstructor();
    makeConnType();
}

IOTV_Host::IOTV_Host(const std::unordered_map<QString, QString> &settingsData, QTcpSocket *reverse_socket, QObject* parent) : Base_Host(0, parent),
    _logDir(settingsData.at(hostField::logDir)), _settingsData(settingsData),
    _counterState(0), _counterPing(0),
    _fragIdent(BUFSIZ * 10)
{
    shareConstructor();
    _conn_type = std::make_unique<TCP_REVERSE_conn_type>(_settingsData[hostField::name], reverse_socket, this);
    makeConnType();
}

void IOTV_Host::shareConstructor()
{
    _timerReRead.setParent(this);
    _timerState.setParent(this);
    _timerPing.setParent(this);

    auto interval = _settingsData[hostField::interval].toUInt();
    _timerReRead.setInterval(interval < 100 ? 100 : interval);
    _timerState.setInterval(TIMER_STATE_INTERVAL);
    _timerPing.setInterval(TIMER_PING_INTERVAL);

    // Используетеся для записи данных полученых от клиентов из других потоков
    connect(this, &IOTV_Host::signalQueryWrite, this, &IOTV_Host::slotQueryWrite, Qt::QueuedConnection);

    connect(&_timerState, &QTimer::timeout, this, &IOTV_Host::slotStateTimeOut, Qt::QueuedConnection);
    connect(&_timerReRead, &QTimer::timeout, this, &IOTV_Host::slotReReadTimeOut, Qt::QueuedConnection);
    connect(&_timerPing, &QTimer::timeout, this, &IOTV_Host::slotPingTimeOut, Qt::QueuedConnection);

    _timerReRead.start();
    _timerState.start();
    _timerPing.start();

    connect(this, &IOTV_Host::signalAddStreamRead, this, &IOTV_Host::slotAddStreamRead, Qt::QueuedConnection);
    connect(this, &IOTV_Host::signalRemoveStreamRead, this, &IOTV_Host::slotRemoveStreamRead, Qt::QueuedConnection);

    connect(this, &IOTV_Host::signalMoveToThread, this, &IOTV_Host::slotMoveToThread, Qt::QueuedConnection);
}

IOTV_Host::~IOTV_Host()
{
    qDebug() << "IOTV_Host destruct";
}

void IOTV_Host::responceIdentification(RAII_Header raii_header)
{
    if (raii_header.header() == nullptr || raii_header.header()->pkg == NULL)
        return;

    const Identification *pkg = static_cast<const Identification *>(raii_header.header()->pkg);
    if (pkg == nullptr)
        return;

    if (raii_header.header()->fragments > 1)
    {
        _fragIdent.addPkg(raii_header);
        if (_fragIdent.isComplete())
        {
            raii_header = _fragIdent.pkg();
            pkg = static_cast<const Identification *>(raii_header.header()->pkg);
            if (pkg == nullptr)
                return;
        }
        else
            return;
    }

    this->setId(pkg->id);

    QString name = QByteArray(pkg->name, pkg->nameSize);
    this->setName(name);
    _logDir.setPath(name);

    this->setDescription(QByteArray{pkg->description, pkg->descriptionSize});
    this->removeAllSubChannel();

    for (uint8_t i = 0; i < pkg->numberReadChannel; i++)
    {
        Q_ASSERT(pkg->readChannelType != NULL);
        this->addReadSubChannel(Raw{static_cast<Raw::DATA_TYPE>(pkg->readChannelType[i])});
    }

    for (uint8_t i = 0; i < pkg->numberWriteChannel; i++)
    {
        Q_ASSERT(pkg->writeChannelType != NULL);
        this->addWriteSubChannel(Raw{static_cast<Raw::DATA_TYPE>(pkg->writeChannelType[i])});
    }

    emit signalIdentRecived();
}

void IOTV_Host::responceState(RAII_iot raii_iot)
{
    if (raii_iot.iot() == nullptr)
        return;

    setState(static_cast<state_t>(raii_iot.iot()->state));
    _counterState = 0;
}

void IOTV_Host::responceRead(RAII_Header raii_header)
{
    if (raii_header.header() == nullptr || raii_header.header()->pkg == NULL)
        return;

    const struct Read_Write *pkg = static_cast<const struct Read_Write *>(raii_header.header()->pkg);
    if (pkg == nullptr)
        return;

    uint8_t channelNumber = pkg->channelNumber;

    if (_streamRead.count(channelNumber))
    {
        emit signalStreamRead(raii_header);
        return;
    }

    QByteArray data(pkg->data, static_cast<qsizetype>(pkg->dataSize));
    this->setReadChannelData(channelNumber, data);

    // Не записываем в лог сырые данные
    if (this->getReadChannelType(channelNumber) ==  Raw::DATA_TYPE::RAW)
        return;

    Raw raw(this->getReadChannelType(pkg->channelNumber), data);
    Log::write(CATEGORY::NONE, "R:"
                                   + QString::number(pkg->channelNumber)
                                   + "="
                                   + raw.strData().first,
               Log::Write_Flag::FILE, logName(pkg->channelNumber));
}

void IOTV_Host::responceWrite(RAII_iot raii_iot) const
{
    if (raii_iot.iot() == nullptr)
        return;
    //Нет никакой реакции на ответ о записи
}

void IOTV_Host::responcePingPong(RAII_iot raii_iot)
{
    if (raii_iot.iot() == nullptr)
        return;

    _counterPing = 0;
}

qint64 IOTV_Host::read(uint8_t channelNumber, readwrite_flag_t flags)
{
    char outData[BUFSIZ];
    auto size = queryReadData(outData, BUFSIZ, getName().toStdString().c_str(), channelNumber, flags);

    return writeToRemoteHost({outData, static_cast<int>(size)}, size);
}

qint64 IOTV_Host::readAll()
{
    return read(0, ReadWrite_FLAGS_IGNORE_CH);
}

qint64 IOTV_Host::write(uint8_t channelNumber, QByteArray data)
{
    if ((channelNumber >= this->getWriteChannelLength()))
        return -1;

    char outData[BUFSIZ];
    auto size = queryWriteData(outData, BUFSIZ, getName().toStdString().c_str(), channelNumber, data.data(), data.size());

    auto resSize = writeToRemoteHost({outData, static_cast<int>(size)}, size);

    emit signalDataTX(channelNumber, data);

    return resSize;
}

QByteArray IOTV_Host::readData(uint8_t channelNumber) const
{
    return this->getReadChannelData(channelNumber);
}

qint64 IOTV_Host::writeToRemoteHost(const QByteArray &data, qint64 size)
{
    // Перехват любой отправки данных, если id ещё не установлен
    if (getId() == 0)
    {
        char outData[BUFSIZ];
        auto sizeData = queryIdentificationData(outData, BUFSIZ);
        return _conn_type->write({outData, static_cast<int>(sizeData)}, sizeData);
    }

    return _conn_type->write(data, size);
}

void IOTV_Host::slotDataResived(QByteArray data)
{
    _counterPing = 0;
    _counterState = 0;

    bool error;
    uint64_t cutDataSize, expectedDataSize;

    _buff += data;

    //!!! Определится с максимальным размером буфера
    if (_buff.size() >= BUFSIZ * 1000)
    {
        Log::write(CATEGORY::ERROR, "slotDataResived HOST переполнение буфера!",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        _buff.clear();
        _conn_type->disconnectFromHost();
        return;
    }

    while (_buff.size() > 0)
    {
        header_t* header = createPkgs(reinterpret_cast<uint8_t*>(_buff.data()), _buff.size(), &error, &expectedDataSize, &cutDataSize);
        RAII_Header raii_header(header);
        clearHeader(header);

        if (error == true)
        {
            _buff.clear();
            break;
        }

        // Пакет не ещё полный
        if (expectedDataSize > 0)
            break;

        if (raii_header.header()->type == HEADER_TYPE_RESPONSE)
        {
            auto *iot = convert();
            RAII_iot raii_iot(iot);
            clear_iotv_obj(iot);

            if (raii_header.header()->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
                responceIdentification(raii_header);
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_READ)
                responceRead(raii_header);
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_WRITE)
                responceWrite(raii_iot);
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_PING_PONG)
                responcePingPong(raii_iot);
            else if (raii_header.header()->assignment == HEADER_ASSIGNMENT_STATE)
            {
                raii_iot.iot()->state = static_cast<const struct State *>(raii_header.header()->pkg)->state;
                responceState(raii_iot);
            }
            //            else if (header->assignment == HEADER_ASSIGNMENT_TECH)
            //                ;
        }
        else if (raii_header.header()->type == HEADER_TYPE_REQUEST)
        {
            // На данный момент устройства нe посылают запросы!!!
            Log::write(CATEGORY::WARNING, "Запрос от устройств не предусмотрен!",
                       Log::Write_Flag::FILE_STDOUT,
                       ServerLog::DEFAULT_LOG_FILENAME);
        }
        _buff = _buff.mid(cutDataSize);
    }
}

QString IOTV_Host::getName() const
{
    if (_conn_type.get() != nullptr)
        return _conn_type->getName();

    return {};
}

void IOTV_Host::setName(const QString &name)
{
    _conn_type->setName(name);
}

//!!!
void IOTV_Host::makeConnType()
{
    const auto &connType = _settingsData[hostField::connection_type];
    if (connType == connectionType::TCP)
    {
        _conn_type = std::make_unique<TCP_conn_type>(_settingsData[hostField::name],
                                                     _settingsData[hostField::address],
                                                     _settingsData[hostField::port].toUInt(),
                                                     this);
    }
    else if (connType == connectionType::TCP_REVERSE)
    {
        ;
    }
    else if (connType == connectionType::UDP)
    {
        _conn_type = std::make_unique<Udp_conn_type>(_settingsData[hostField::name],
                                                     _settingsData[hostField::address],
                                                     _settingsData[hostField::port].toUInt(),
                                                     this);
    }
    else if (connType == connectionType::COM)
    {
        _conn_type = std::make_unique<COM_conn_type>(_settingsData["name"], this);

        //    COM_conn_type *com = qobject_cast<COM_conn_type*>(_conn_type.get());
        //    com->setAddress(_structSettings->address);
        //    com->setSettingsPort(settingPort);

        //        COM_conn_type::SetingsPort settingsPort;
        //        settingsPort.baudRate = _settingsHosts.value("baudRate", 115200).toInt();
        //        settingsPort.dataBits = _settingsHosts.value("dataBits", 8).toInt();
        //        settingsPort.parity = _settingsHosts.value("parity", 0).toInt();
        //        settingsPort.stopBits = _settingsHosts.value("stopBits", 1).toInt();
        //        settingsPort.flowControl = _settingsHosts.value("flowControl", 0).toInt();
    }
    else if (connType == connectionType::FILE)
        _conn_type = std::make_unique<File_conn_type>(_settingsData["name"], _settingsData["address"], this);
    else
        Q_ASSERT(false);

    connect(_conn_type.get(), &Base_conn_type::signalConnected, this, &IOTV_Host::slotConnected);
    connect(_conn_type.get(), &Base_conn_type::signalConnected, this, &Base_Host::signalConnected);
    connect(_conn_type.get(), &Base_conn_type::signalDisconnected, this, &Base_Host::signalDisconnected);
    connect(_conn_type.get(), &Base_conn_type::signalDataRiceved, this, &IOTV_Host::slotDataResived);

    _conn_type->connectToHost();
}

const std::unordered_map<QString, QString> &IOTV_Host::settingsData() const
{
    return _settingsData;
}

bool IOTV_Host::addStreamRead(uint8_t channel, const QString &client)
{
    char outData[BUFSIZ];
    bool result = false;

    if (_streamRead.count(channel) == 0)
    {
        _streamRead[channel] = {client};
        result = true;

        auto size = queryReadData(outData, BUFSIZ, this->getName().toStdString().c_str(), channel, ReadWrite_FLAGS_OPEN_STREAM);
        writeToRemoteHost({outData, static_cast<int>(size)}, size);
    }
    else
        result = _streamRead[channel].insert(client).second;

    return result;
}

//void IOTV_Host::addStreamWrite(uint8_t channel)
//{
//    std::lock_guard lg(_mutexStreamWrite);
//    _streamWrite.insert(channel);
//}

void IOTV_Host::removeStreamRead(uint8_t channel, const QString &client)
{
    char outData[BUFSIZ];

    if (_streamRead.count(channel) == 0)
        return;

    _streamRead[channel].erase(client);

    if (_streamRead[channel].size() == 0)
    {
        _streamRead.erase(channel);
        auto size = queryReadData(outData, BUFSIZ, this->getName().toStdString().c_str(), channel, ReadWrite_FLAGS_CLOSE_STREAM);
        writeToRemoteHost({outData, static_cast<int>(size)}, size);
    }
}

QString IOTV_Host::getAddress() const
{
    return _conn_type->getAddress();
}

//void IOTV_Host::removeStreamWrite(uint8_t channel)
//{
//    std::lock_guard lg(_mutexStreamWrite);
//    _streamWrite.erase(channel);
//}

void IOTV_Host::slotReReadTimeOut()
{
    readAll();
}

void IOTV_Host::slotStateTimeOut()
{
    ++_counterState;

    char outData[BUFSIZ];
    auto size = queryStateData(outData, BUFSIZ, getName().toStdString().c_str());
    writeToRemoteHost({outData, static_cast<int>(size)}, size);

    if (_counterState > COUNTER_STATE_COUNT)
    {
        setState(State_STATE_OFFLINE);
        _counterState = 0;
    }
}

void IOTV_Host::slotPingTimeOut()
{
    //    qDebug() << "ping";
    ++_counterPing;

    char outData[BUFSIZ];
    auto size = queryPingData(outData, BUFSIZ);
    writeToRemoteHost({outData, static_cast<int>(size)}, size);

    if (_counterPing > COUNTER_PING_COUNT)
    {
        Log::write(CATEGORY::WARNING, _conn_type->getName() + " ping timeout",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
        _conn_type->connectToHost();
        emit signalDevicePingTimeOut();
        _counterPing = 0;
    }
}

void IOTV_Host::slotQueryWrite(int channelNumber, QByteArray data)
{
    write(channelNumber, std::move(data));

    // Не записываем в лог сырые данные
    if (this->getReadChannelType(channelNumber) ==  Raw::DATA_TYPE::RAW)
        return;

    Raw raw(this->getReadChannelType(channelNumber), data);
    Log::write(CATEGORY::NONE, "W:"
                                   + QString::number(channelNumber)
                                   + "="
                                   + raw.strData().first,
               Log::Write_Flag::FILE, logName(channelNumber));
}

void IOTV_Host::slotConnected()
{
    _timerReRead.start();
    _timerState.start();
    _timerPing.start();

    _counterPing = 0;
    _counterState = 0;
    _buff.clear();

    _streamRead.clear();
    _streamWrite.clear();
}

void IOTV_Host::slotMoveToThread(QThread *newThread)
{
    QThread *curThread = QThread::currentThread();
    this->moveToThread(newThread);
    qDebug() << "host moved to thread";
    emit signalMovedToThread(curThread);
}

void IOTV_Host::slotDisconnected()
{
    _conn_type->disconnectFromHost();
}

void IOTV_Host::slotAddStreamRead(uint8_t channel, QString client)
{
    if (addStreamRead(channel, client) == false)
    {
        QString msg = "Add read stream wrong. Channel #" + QString::number(channel);
        Log::write(CATEGORY::ERROR, msg, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
    }
}

void IOTV_Host::slotRemoveStreamRead(uint8_t channel, QString client)
{
    removeStreamRead(channel, client);
}
