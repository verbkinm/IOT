#include "iotv_host.h"

IOTV_Host::IOTV_Host(std::unordered_map<QString, QString> &settingsData, QObject* parent) : Base_Host(0, parent),
    _logFile(settingsData[hostField::logFile]), _settingsData(settingsData), _parentThread(QThread::currentThread()),
    _expectedDataSize(0)
{
    _timerPing.setParent(this);
    _timerPong.setParent(this);
    _timerReRead.setParent(this);

    connect(&_thread, &QThread::started, this, &IOTV_Host::slotNewThreadStart, Qt::QueuedConnection);
    connect(this, &IOTV_Host::signalStopThread, this, &IOTV_Host::slotThreadStop, Qt::QueuedConnection);

    connect(this, &IOTV_Host::signalQueryWrite, this, &IOTV_Host::slotQueryWrite, Qt::QueuedConnection);
}

IOTV_Host::~IOTV_Host()
{
    emit signalStopThread();
    _thread.wait();
}

void IOTV_Host::setOnline(bool state)
{
    std::lock_guard lg(_mutexParametersChange);
    _state_flags.setFlag(DeviceOnline, state);
}

void IOTV_Host::responceIdentification(const struct Header *header)
{
    Q_ASSERT(header != NULL);
    Q_ASSERT(header->identification != NULL);

    this->setId(header->identification->id);
    // На данный момент имя константное и считывается с файла настроек
//    this->setNname
    this->setDescription(header->identification->description);
    this->removeAllSubChannel();

    for (uint8_t i = 0; i < header->identification->numberReadChannel; i++)
    {
        Q_ASSERT(header->identification->readChannelType != NULL);
        Raw rawData(static_cast<Raw::DATA_TYPE>(header->identification->readChannelType[i]));
        this->addReadSubChannel(rawData);
    }

    for (uint8_t i = 0; i < header->identification->numberWriteChannel; i++)
    {
        Q_ASSERT(header->identification->writeChannelType != NULL);
        Raw rawData(static_cast<Raw::DATA_TYPE>(header->identification->writeChannelType[i]));
        this->addWriteSubChannel(rawData);
    }

    _timerPing.start(TIMER_PING);
    _timerReRead.start();
}

void IOTV_Host::responceState(struct IOTV_Server_embedded *iot)
{
    Q_ASSERT(iot != nullptr);
//    if (iot == nullptr)
//        return;

    this->setState(static_cast<Base_Host::STATE>(iot->state));
}

void IOTV_Host::responceRead(const struct Header *header)
{
    Q_ASSERT(header != nullptr);
    Q_ASSERT(header->readWrite != nullptr);

    QByteArray data(header->readWrite->data, header->readWrite->dataSize);
    this->setReadChannelData(header->readWrite->channelNumber, data);

    if(_logFile.isEmpty())
        return;

    Raw raw(this->getReadChannelType(header->readWrite->channelNumber), data);
    Log::write("R:"
               + QString::number(header->readWrite->channelNumber)
               + "="
               + raw.strData().first,
               Log::Write_Flag::FILE, _logFile);
}

void IOTV_Host::responceWrite(struct IOTV_Server_embedded *iot)
{
    Q_ASSERT(iot != nullptr);
    //Нет никакой реакции на ответ о записи
}

void IOTV_Host::responcePingPoing(struct IOTV_Server_embedded *iot)
{
    Q_ASSERT(iot != nullptr);

    _timerPong.start(TIMER_PONG);
    _timerPing.start(TIMER_PING);
}

bool IOTV_Host::isOnline() const
{
    return _state_flags.testFlag(Flag::DeviceOnline);
}

qint64 IOTV_Host::read(uint8_t channelNumber)
{
    if(!isOnline())
        return -1;

    char outData[BUFSIZ];
    auto size = queryReadData(outData, BUFSIZ, getName().toStdString().c_str(), channelNumber);

    return writeToRemoteHost({outData, static_cast<qsizetype>(size)});
}

qint64 IOTV_Host::write(uint8_t channelNumber, const QByteArray &data)
{
    if(!isOnline() || (channelNumber >= this->getWriteChannelLength()))
        return -1;

    char outData[BUFSIZ];
    auto size = queryWriteData(outData, BUFSIZ, getName().toStdString().c_str(), channelNumber, data.data(), data.size());

    return writeToRemoteHost({outData, static_cast<qsizetype>(size)});
}

QByteArray IOTV_Host::readData(uint8_t channelNumber) const
{
    return this->getReadChannelData(channelNumber);
}

qint64 IOTV_Host::writeToRemoteHost(const QByteArray &data, qint64 size)
{
    std::lock_guard lg(_mutexWrite);
    return _conn_type->write(data, size);
}

void IOTV_Host::dataResived(QByteArray data)
{
    bool error = false;
    uint64_t cutDataSize = 0;

    while (data.size() > 0)
    {
        struct Header* header = createPkgs(reinterpret_cast<uint8_t*>(data.data()), data.size(), &error, &_expectedDataSize, &cutDataSize);

        if (error == true)
        {
            _conn_type->clearDataBuffer();
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
            struct IOTV_Server_embedded *iot = convert();

            if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
                responceIdentification(header);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_READ)
                responceRead(header);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
                responceWrite(iot);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_PING_PONG)
                responcePingPoing(iot);
            else if(header->assignment == Header::HEADER_ASSIGNMENT_STATE)
                responceState(iot);

//            printHeader(header);

            clearIOTV_Server(iot);
        }
        else if(header->type == Header::HEADER_TYPE_REQUEST)
        {
            // На данный момент устройства нe посылают запросы!!!
            Log::write("Запрос от устройств не предусмотрен!");
        }

        data = data.mid(cutDataSize);

        clearHeader(header);
    }

    _conn_type->setDataBuffer(data);
}

QString IOTV_Host::getName() const
{
    if (_conn_type.get() != nullptr)
        return _conn_type->getName();

    return {};
}

void IOTV_Host::connectToHost()
{
    _conn_type->connectToHost();
}

//!!!
void IOTV_Host::setConnectionType()
{
    const auto &connType = _settingsData[hostField::connection_type];
    if (connType == connectionType::TCP)
    {
        _conn_type = std::make_unique<TCP_conn_type>(_settingsData[hostField::name],
                _settingsData[hostField::address],
                _settingsData[hostField::port].toUInt(),
                this);
    }
    else if (connType == connectionType::UDP)
        //!!!
        ;
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

    connect(_conn_type.get(), &Base_conn_type::signalConnected, this, &IOTV_Host::slotConnected, Qt::QueuedConnection);
    connect(_conn_type.get(), &Base_conn_type::signalDisconnected, this, &IOTV_Host::slotDisconnected, Qt::QueuedConnection);
    connect(_conn_type.get(), &Base_conn_type::signalDataRiceved, this, &IOTV_Host::dataResived, Qt::QueuedConnection);
}

bool IOTV_Host::runInNewThread()
{
    if (_parentThread == &_thread)
        return false;

    this->moveToThread(&_thread);

    _thread.start();

    return _thread.isRunning();
}

const std::unordered_map<QString, QString> &IOTV_Host::settingsData() const
{
    return _settingsData;
}

void IOTV_Host::slotConnected()
{
    setOnline(true);

    char outData[BUFSIZ];
    auto size = queryIdentificationData(outData, BUFSIZ);

    writeToRemoteHost({outData, static_cast<qsizetype>(size)});

    _timerPong.start(TIMER_PONG);
}

void IOTV_Host::slotDisconnected()
{
    setOnline(false);

    _timerPing.stop();
    _timerPong.stop();
    _timerReRead.stop();

    _conn_type->clearDataBuffer();
    _expectedDataSize = 0;
}

void IOTV_Host::slotReReadTimeOut()
{
    for (int i = 0; i < getReadChannelLength(); i++)
        read(i);
}

void IOTV_Host::slotPingTimeOut()
{
    char outData[BUFSIZ];
    auto size = queryPingData(outData, BUFSIZ);

    writeToRemoteHost({outData, static_cast<qsizetype>(size)});
}

void IOTV_Host::slotReconnectTimeOut()
{
    _conn_type->disconnectFromHost();
    Log::write(_conn_type->getName() + " WARRNING: ping timeout");
}

void IOTV_Host::slotNewThreadStart()
{
    connect(&_timerPing, &QTimer::timeout, this, &IOTV_Host::slotPingTimeOut, Qt::QueuedConnection);
    connect(&_timerPong, &QTimer::timeout, this, &IOTV_Host::slotReconnectTimeOut, Qt::QueuedConnection);
    connect(&_timerReRead, &QTimer::timeout, this, &IOTV_Host::slotReReadTimeOut, Qt::QueuedConnection);

    auto interval = _settingsData[hostField::interval].toUInt();
    _timerReRead.setInterval(interval < 1000 ? 1000 : interval);

    setConnectionType();

    connectToHost();
}

void IOTV_Host::slotThreadStop()
{
    if (_parentThread == nullptr)
        return;

    this->moveToThread(_parentThread);

    _timerPing.moveToThread(_parentThread);
    _timerPong.moveToThread(_parentThread);
    _timerReRead.moveToThread(_parentThread);

    _thread.exit();
}

void IOTV_Host::slotQueryWrite(int channelNumber, QByteArray data)
{
    write(channelNumber, data);
}
