#include "iotv_host.h"

IOTV_Host::IOTV_Host(std::unordered_map<QString, QString> &settingsData, QObject* parent) : Base_Host(0, parent),
    _logFile(settingsData[hostField::logFile]), _settingsData(settingsData), _parentThread(QThread::currentThread())
{
    _timerPing.setParent(this);
    _timerPong.setParent(this);
    _reReadTimer.setParent(this);

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

void IOTV_Host::responceRequest(std::unique_ptr<IOTVP_Header> header)
{
    Q_UNUSED(header);
    // На данный момент устройства на кидают запросы!!!
    Log::write("Запрос от устройств не предусмотрен!");
}

void IOTV_Host::responceResponce(std::unique_ptr<IOTVP_Header> header)
{
    switch (header->assignment())
    {
    case IOTVP_Header::ASSIGNMENT::IDENTIFICATION:
        responceIdentification(std::move(header));
        break;
    case IOTVP_Header::ASSIGNMENT::STATE:
        responceState(std::move(header));
        break;
    case IOTVP_Header::ASSIGNMENT::READ:
        responceRead(std::move(header));
        break;
    case IOTVP_Header::ASSIGNMENT::WRITE:
        responceWrite(std::move(header));
        break;
    case IOTVP_Header::ASSIGNMENT::PING_PONG:
        responcePingPoing(std::move(header));
        break;
    default:
        Log::write("Неизвестный тип назначения!");
        break;
    }
}

void IOTV_Host::responceIdentification(std::unique_ptr<IOTVP_Header> header)
{
    Q_ASSERT(header != nullptr);

    auto body = header->takeBody();

    const IOTVP_Identification  *identification= dynamic_cast<const IOTVP_Identification *>(body.get());
    Q_ASSERT(identification != nullptr);

    this->setId(identification->id());
    this->setDescription(identification->description());
    this->removeAllSubChannel();

    for (uint8_t i = 0; i < identification->numberReadChannel(); i++)
        this->addReadSubChannel(identification->readChannel().at(i));

    for (uint8_t i = 0; i < identification->numberWriteChannel(); i++)
        this->addWriteSubChannel(identification->writeChannel().at(i));

    _timerPing.start(TIMER_PING);
    _reReadTimer.start();
}

void IOTV_Host::responceState(std::unique_ptr<IOTVP_Header> header)
{
    Q_ASSERT(header != nullptr);

    auto body = header->takeBody();

    const IOTVP_State  *ptrState = dynamic_cast<const IOTVP_State *>(body.get());
    Q_ASSERT(ptrState != nullptr);

    this->setState(static_cast<Base_Host::STATE>(ptrState->state()));
}

void IOTV_Host::responceRead(std::unique_ptr<IOTVP_Header> header)
{
    Q_ASSERT(header != nullptr);

    auto body = header->takeBody();

    const IOTVP_READ_WRITE  *read = dynamic_cast<const IOTVP_READ_WRITE *>(body.get());
    Q_ASSERT(read != nullptr);

    this->setReadChannelData(read->channelNumber(), read->data());

    if(_logFile.isEmpty())
        return;

    Raw raw(this->getReadChannelType(read->channelNumber()), read->data());
    Log::write("R:"
               + QString::number(read->channelNumber())
               + "="
               + raw.strData().first,
               Log::Write_Flag::FILE, _logFile);
}

void IOTV_Host::responceWrite(std::unique_ptr<IOTVP_Header> header)
{
    Q_ASSERT(header != nullptr);
    //Нет никакой реакции на ответ о записи

}

void IOTV_Host::responcePingPoing(std::unique_ptr<IOTVP_Header> header)
{
    Q_ASSERT(header != nullptr);

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

    IOTVP_Creator creator();
    return  writeToRemoteHost(IOTV_SH::query_READ(channelNumber));
}

qint64 IOTV_Host::write(uint8_t channelNumber, const QByteArray &data)
{
    if(!isOnline() || (channelNumber >= this->getWriteChannelLength()))
        return -1;

    return writeToRemoteHost(IOTV_SH::query_WRITE(channelNumber, data));
}

QByteArray IOTV_Host::readData(uint8_t channelNumber) const
{
    return this->getReadChannelData(channelNumber);
}

qint64 IOTV_Host::writeToRemoteHost(const QByteArray &data)
{
    std::lock_guard lg(_mutexWrite);
    return _conn_type->write(data);
}

void IOTV_Host::dataResived(QByteArray data)
{
    if (static_cast<uint64_t>(data.size()) < _expectedDataSize)
        return;



    while(data.size() > 0)
    {
        IOTVP_Creator creator(data);
        creator.createPkgs();

        // Ошибка данных
        if (creator.error())
        {
            _expectedDataSize = 0;
            _conn_type->clearDataBuffer();
            break;
        }
        // Пакет данных не полный
        if (creator.expectedDataSize() > 0)
        {
            _expectedDataSize = creator.expectedDataSize();
            break;
        }

        // Обрезаем данные
        data = data.mid(creator.cutDataSize()); // !!! sliced
        _conn_type->setDataBuffer(data);

        auto header = creator.takeHeader();
        if (header->type() == IOTVP_Header::TYPE::REQUEST)
            responceRequest(std::move(header));
        else if (header->type() == IOTVP_Header::TYPE::RESPONSE)
            responceResponce(std::move(header));
    }

    bool error = false;
    uint64_t cutDataSize = 0;

    while (data.size() > 0)
    {
        struct Header* header = createPkgs(reinterpret_cast<uint8_t*>(data.data()), data.size(), &error, &_conn_type->expectedDataSize, &cutDataSize);

        if (error == true)
        {
            _conn_type->clearDataBuffer();
            //            data.clear();
            _conn_type->expectedDataSize = 0;
            //        cutDataSize = 0;
            clearHeader(header);
            break;
        }

        if (_conn_type->expectedDataSize > 0)
        {
            clearHeader(header);
            break;
        }

        if (header->type == Header::HEADER_TYPE_REQUEST)
        {
//            IOTV_Server iot = {

//            }
            if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
            {
                uint64_t size = responseIdentificationData(_conn_type->transmitBuffer, BUFSIZ, &iot);
//                _socket->write(_conn_type->transmitBuffer, size);
            }
            else if(header->assignment == Header::HEADER_ASSIGNMENT_READ)
            {
                /* uint64_t size = */responseReadData(_conn_type->transmitBuffer, BUFSIZ, &iot, header);
                //            socket->write(transmitBuffer, size);
            }
            else if(header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
            {
                /* uint64_t size = */responseWriteData(_conn_type->transmitBuffer, BUFSIZ, &iot, header);
                //            socket->write(transmitBuffer, size);
            }
            else if(header->assignment == Header::HEADER_ASSIGNMENT_PING_PONG)
            {
                /*uint64_t size = */responsePingData(_conn_type->transmitBuffer, BUFSIZ);
                //            socket->write(transmitBuffer, size);
            }
            else if(header->assignment == Header::HEADER_ASSIGNMENT_STATE)
            {
                /*uint64_t size = */responseStateData(_conn_type->transmitBuffer, BUFSIZ, &iot, header);
                //            socket->write(transmitBuffer, size);
            }
        }

        //!!!
        memmove((void*)recivedBuffer, (void*)&recivedBuffer[cutDataSize], BUFSIZ - cutDataSize);
        realBufSize -= cutDataSize; // тут всегда должно уходить в ноль, если приём идёт по 1 байту!

        clearHeader(header);
    }


    //    int dataSize = data.size();

    //    IOTV_SH::RESPONSE_PKG *pkg;
    //    //!!! переделать логику с trimBufferFromBegin, возможно удалить его вовсе
    //    while ((pkg = IOTV_SH::accumPacket(data)) != nullptr)
    //    {
    //        if ((pkg->type == IOTV_SH::Response_Type::RESPONSE_INCOMPLETE) ||
    //                ((pkg->type == IOTV_SH::Response_Type::RESPONSE_ERROR) && (data.size() == 0)))
    //        {
    //            delete pkg;
    //            return;
    //        }

    //        this->_conn_type->clearDataBuffer(dataSize - data.size());

    //        if (pkg->type == IOTV_SH::Response_Type::RESPONSE_WAY)
    //            response_WAY_recived(pkg);
    //        else if (pkg->type == IOTV_SH::Response_Type::RESPONSE_PONG)
    //            response_PONG_recived(pkg);
    //        else if (pkg->type == IOTV_SH::Response_Type::RESPONSE_READ)
    //            response_READ_recived(pkg);
    //        else if (pkg->type == IOTV_SH::Response_Type::RESPONSE_WRITE)
    //            response_WRITE_recived(pkg);
    //        else if (pkg->type == IOTV_SH::Response_Type::RESPONSE_ERROR)
    //        {
    //            this->_conn_type->clearDataBuffer(1);
    //            data = data.mid(1);
    //        }
    //        else
    //            Log::write(_conn_type->getName() + " WARRNING: received data UNKNOW: " + data.toHex(':'));

    //        delete pkg;
    //    }
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

//void IOTV_Host::response_WAY_recived(const IOTV_SH::RESPONSE_PKG *pkg)
//{
//    if (pkg == nullptr)
//        return;

//    if (pkg->type != IOTV_SH::Response_Type::RESPONSE_WAY)
//    {
//        delete pkg;
//        return;
//    }

//    const IOTV_SH::RESPONSE_WAY *wayPkg = static_cast<const IOTV_SH::RESPONSE_WAY*>(pkg);

//    this->setId(wayPkg->id);
//    this->setDescription(wayPkg->description);

//    this->removeAllSubChannel();

//    for (uint8_t i = 0; i < wayPkg->readChannel.size(); i++)
//        this->addReadSubChannel({wayPkg->readChannel.at(i)});

//    for (uint8_t i = 0; i < wayPkg->writeChannel.size(); i++)
//        this->addWriteSubChannel({wayPkg->writeChannel.at(i)});

//    _timerPing.start(TIMER_PING);
//    _reReadTimer.start();
//}

//void IOTV_Host::response_READ_recived(const IOTV_SH::RESPONSE_PKG *pkg)
//{
//    if (pkg == nullptr)
//        return;

//    if (pkg->type != IOTV_SH::Response_Type::RESPONSE_READ)
//    {
//        delete pkg;
//        return;
//    }

//    const IOTV_SH::RESPONSE_READ *readPkg = static_cast<const IOTV_SH::RESPONSE_READ *>(pkg);

//    this->setReadChannelData(readPkg->chanelNumber, readPkg->data);

//    if(_logFile.isEmpty())
//        return;

//    Raw raw(this->getReadChannelType(readPkg->chanelNumber), readPkg->data);
//    Log::write("R:"
//               + QString::number(readPkg->chanelNumber)
//               + "="
//               + raw.strData().first,
//               Log::Write_Flag::FILE, _logFile);
//}

//void IOTV_Host::response_WRITE_recived(const IOTV_SH::RESPONSE_PKG *pkg)
//{
//    if (pkg == nullptr)
//        return;

//    if (pkg->type != IOTV_SH::Response_Type::RESPONSE_WRITE)
//    {
//        delete pkg;
//        return;
//    }
//    //Нет никакой реакции на ответ о записи
//    Q_UNUSED(pkg)
//}

//void IOTV_Host::response_PONG_recived(const IOTV_SH::RESPONSE_PKG *pkg)
//{
//    if (pkg == nullptr)
//        return;

//    if (pkg->type != IOTV_SH::Response_Type::RESPONSE_PONG)
//    {
//        delete pkg;
//        return;
//    }

//    const IOTV_SH::RESPONSE_PONG *pongPkg = static_cast<const IOTV_SH::RESPONSE_PONG *>(pkg);

//    if (pongPkg->state)
//    {
//        _timerPong.start(TIMER_PONG);
//        _timerPing.start(TIMER_PING);
//    }
//}

const std::unordered_map<QString, QString> &IOTV_Host::settingsData() const
{
    return _settingsData;
}

void IOTV_Host::slotConnected()
{
    setOnline(true);
    _conn_type->write(IOTV_SH::query_WAY());

    _timerPong.start(TIMER_PONG);
}

void IOTV_Host::slotDisconnected()
{
    setOnline(false);

    _timerPing.stop();
    _timerPong.stop();
    _reReadTimer.stop();

    _conn_type->clearDataBuffer();
}

void IOTV_Host::slotReReadTimeOut()
{
    for (int i = 0; i < getReadChannelLength(); i++)
        read(i);
}

void IOTV_Host::slotPingTimeOut()
{
    QByteArray data;
    data.push_back(IOTV_SH::QUERY_PING_BYTE);
    _conn_type->write(data);
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
    connect(&_reReadTimer, &QTimer::timeout, this, &IOTV_Host::slotReReadTimeOut, Qt::QueuedConnection);

    auto interval = _settingsData[hostField::interval].toUInt();
    _reReadTimer.setInterval(interval < 1000 ? 1000 : interval);

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
    _reReadTimer.moveToThread(_parentThread);

    _thread.exit();
}

void IOTV_Host::slotQueryWrite(int channelNumber, QByteArray data)
{
    write(channelNumber, data);
}
