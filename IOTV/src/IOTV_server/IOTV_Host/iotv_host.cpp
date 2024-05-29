#include "iotv_host.h"

#include <QFileInfo>
#include <QDate>

IOTV_Host::IOTV_Host(const std::unordered_map<QString, QString> &settingsData, QObject* parent) : Base_Host(0, parent),
    _logDir(settingsData.at(hostField::logDir)),
    _settingsData(settingsData),
    _counterState(0), _counterPing(0)
{
    shareConstructor();
    makeConnType();
}

IOTV_Host::IOTV_Host(const std::unordered_map<QString, QString> &settingsData, QTcpSocket *reverse_socket, QObject* parent) : Base_Host(0, parent),
    _logDir(settingsData.at(hostField::logDir)), _settingsData(settingsData),
    _counterState(0), _counterPing(0)
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
}

IOTV_Host::~IOTV_Host()
{
    qDebug() << "IOTV_Host destruct";
}

void IOTV_Host::responceIdentification(const header_t *header)
{
//    qDebug() << __FUNCTION__;
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->pkg != NULL);

    const struct Identification *pkg = static_cast<const struct Identification *>(header->pkg);

    this->setId(pkg->id);
    this->setName(pkg->name);
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

void IOTV_Host::responceState(const iotv_obj_t *iot)
{
    Q_ASSERT(iot != nullptr);
    setState(static_cast<state_t>(iot->state));
    _counterState = 0;
}

void IOTV_Host::responceRead(const header_t *header)
{
    Q_ASSERT(header != nullptr);
    Q_ASSERT(header->pkg != nullptr);

    const struct Read_Write *pkg = static_cast<const struct Read_Write *>(header->pkg);

    uint8_t channelNumber = pkg->channelNumber;

//    if (channelNumber == 40)
//    {
////        printf("it = NULL\n");
//    }

    if (_streamRead.count(channelNumber))
    {
        emit signalStreamRead(channelNumber, header->fragment, header->fragments, {pkg->data, static_cast<qsizetype>(pkg->dataSize)});
        return;
    }

    QByteArray data(pkg->data, static_cast<qsizetype>(pkg->dataSize));
    this->setReadChannelData(channelNumber, data);

//    if (channelNumber == 40)
//    {
//        qDebug() << getReadChannelData(40).size() << getReadChannelData(40);
//    }

    // Не записываем в лог сырые данные
    if (this->getReadChannelType(channelNumber) ==  Raw::DATA_TYPE::RAW)
        return;

    Raw raw(this->getReadChannelType(pkg->channelNumber), data);
    Log::write("R:"
                   + QString::number(pkg->channelNumber)
                   + "="
                   + raw.strData().first,
               Log::Write_Flag::FILE, logName(pkg->channelNumber));
}

void IOTV_Host::responceWrite(const iotv_obj_t *iot) const
{
    Q_ASSERT(iot != nullptr);
    //Нет никакой реакции на ответ о записи
}

void IOTV_Host::responcePingPong(const iotv_obj_t *iot)
{
    Q_ASSERT(iot != nullptr);
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
        Log::write("slotDataResived HOST переполнение буфера!",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        _buff.clear();
        _conn_type->disconnectFromHost();
        return;
    }

    while (_buff.size() > 0)
    {
        header_t* header = createPkgs(reinterpret_cast<uint8_t*>(_buff.data()), _buff.size(), &error, &expectedDataSize, &cutDataSize);

        if (error == true)
        {
            _buff.clear();
            clearHeader(header);
            break;
        }

        // Пакет не ещё полный
        if (expectedDataSize > 0)
        {
            clearHeader(header);
            break;
        }

        if (header->type == HEADER_TYPE_RESPONSE)
        {
            auto *iot = convert();

            if (header->assignment == HEADER_ASSIGNMENT_IDENTIFICATION)
                responceIdentification(header);
            else if (header->assignment == HEADER_ASSIGNMENT_READ)
                responceRead(header);
            else if (header->assignment == HEADER_ASSIGNMENT_WRITE)
                responceWrite(iot);
            else if (header->assignment == HEADER_ASSIGNMENT_PING_PONG)
                responcePingPong(iot);
            else if (header->assignment == HEADER_ASSIGNMENT_STATE)
            {
                iot->state = static_cast<const struct State *>(header->pkg)->state;
                responceState(iot);
            }
            //            else if (header->assignment == HEADER_ASSIGNMENT_TECH)
            //                ;

            clear_iotv_obj(iot);
        }
        else if (header->type == HEADER_TYPE_REQUEST)
        {
            // На данный момент устройства нe посылают запросы!!!
            Log::write("Запрос от устройств не предусмотрен!",
                       Log::Write_Flag::FILE_STDOUT,
                       ServerLog::DEFAULT_LOG_FILENAME);
        }
        _buff = _buff.mid(cutDataSize);
        clearHeader(header);
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

bool IOTV_Host::addStreamRead(uint8_t channel, QObject *client)
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

void IOTV_Host::removeStreamRead(uint8_t channel, QObject *client)
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
        Log::write(_conn_type->getName() + " WARRNING: ping timeout",
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
    Log::write("W:"
                   + QString::number(channelNumber)
                   + "="
                   + raw.strData().first,
               Log::Write_Flag::FILE, logName(channelNumber));
}

void IOTV_Host::slotConnected()
{
    //    qDebug() << this->thread() << _timerPing.thread();
    _timerReRead.start();
    _timerState.start();
    _timerPing.start();

    _counterPing = 0;
    _counterState = 0;
    _buff.clear();

    _streamRead.clear();
    _streamWrite.clear();
}

void IOTV_Host::slotDisconnected()
{
    _conn_type->disconnectFromHost();
}

void IOTV_Host::slotAddStreamRead(uint8_t channel, QObject *client)
{
    if (addStreamRead(channel, client) == false)
    {
        QString msg = "Add read stream wrong. Channel #" + QString::number(channel);
        Log::write(msg, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
    }
}

void IOTV_Host::slotRemoveStreamRead(uint8_t channel, QObject *client)
{
    removeStreamRead(channel, client);
}
