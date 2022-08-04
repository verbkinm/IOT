#include "../Protocols/IOTV_SH.h"

#include "IOT_Host.h"

IOT_Host::IOT_Host(IOT_Host_StructSettings &structSettings, QObject* parent) : Base_Host(0), QObject(parent),
    _structSettings(structSettings)
{
    connect(&_thread, &QThread::started, this, &IOT_Host::slotNewThreadStart, Qt::QueuedConnection);
}

IOT_Host::~IOT_Host()
{
    //!!!
    _thread.quit();
}

Base_conn_type::Conn_type IOT_Host::getConnectionType() const
{
    return _conn_type->getConnectionType();
}

void IOT_Host::setOnline(bool state)
{
    std::lock_guard lg(_mutexParametersChange);
    _state_flags.setFlag(DeviceOnline, state);
}

bool IOT_Host::isOnline() const
{
    return _state_flags.testFlag(Flag::DeviceOnline);
}

qint64 IOT_Host::readData(uint8_t channelNumber)
{
    if(!isOnline())
        return -1;

    QByteArray data = IOTV_SH::query_READ(channelNumber);
    return  writeToServer(data);
}

qint64 IOT_Host::writeData(uint8_t channelNumber, const Raw &data)
{
    if(!isOnline())
        return -1;

    return writeToServer(IOTV_SH::query_WRITE(channelNumber, data));
}

qint64 IOT_Host::writeToServer(const QByteArray &data)
{
    return _conn_type->write(data);
}

void IOT_Host::dataResived(const IOTV_SH::RESPONSE_PKG &data)
{
    if(data.type == IOTV_SH::Response_Type::RESPONSE_WAY)
        response_WAY_recived(data);
    else if(data.type == IOTV_SH::Response_Type::RESPONSE_PONG)
        response_PONG_recived(data);
    else if(data.type == IOTV_SH::Response_Type::RESPONSE_READ)
        response_READ_recived(data);
    else if(data.type == IOTV_SH::Response_Type::RESPONSE_WRITE)
        response_WRITE_recived(data);
    else
        Log::write(_conn_type->getName() + " WARRNING: received data UNKNOW: ");
}

std::string IOT_Host::getName() const
{
    return _conn_type.get()->getName().toStdString();
}

void IOT_Host::setConnectionTypeTCP()
{
    _conn_type = std::make_unique<TCP_conn_type>(_structSettings.name);

    TCP_conn_type* eth = qobject_cast<TCP_conn_type*>(_conn_type.get());
    eth->setAddress(_structSettings.address);
    eth->setPort(_structSettings.port);

    connectObjects();
}

void IOT_Host::setConnectionTypeCom(const COM_conn_type::SetingsPort &settingPort)
{
    _conn_type = std::make_unique<COM_conn_type>(_structSettings.name);

    COM_conn_type *com = qobject_cast<COM_conn_type*>(_conn_type.get());
    com->setAddress(_structSettings.address);
    com->setSettingsPort(settingPort);

    connectObjects();
}

void IOT_Host::setConnectionTypeFile()
{
    _conn_type = std::make_unique<File_conn_type>(_structSettings.name, _structSettings.address);
    connectObjects();
}

void IOT_Host::connectToHost()
{
    _conn_type->connectToHost();
}

bool IOT_Host::runInNewThread()
{
    this->moveToThread(&_thread);

    _timerPing.moveToThread(&_thread);
    _timerReconnect.moveToThread(&_thread);
    _reReadTimer.moveToThread(&_thread);

    _thread.start();

    return _thread.isRunning();
}

void IOT_Host::connectObjects() const
{
    connect(_conn_type.get(), &Base_conn_type::signalConnected, this, &IOT_Host::slotConnected, Qt::QueuedConnection);
    connect(_conn_type.get(), &Base_conn_type::signalDisconnected, this, &IOT_Host::slotDisconnected, Qt::QueuedConnection);
    connect(_conn_type.get(), &Base_conn_type::signalDataRiceved, this, &IOT_Host::dataResived, Qt::QueuedConnection);
}

void IOT_Host::response_WAY_recived(const IOTV_SH::RESPONSE_PKG &pkg)
{
    if (pkg.type != IOTV_SH::Response_Type::RESPONSE_WAY)
        return;

    const IOTV_SH::RESPONSE_WAY &wayPkg = static_cast<const IOTV_SH::RESPONSE_WAY &>(pkg);

    this->setId(wayPkg.id);
    this->setDescription(wayPkg.description);

    for (uint8_t i = 0; i < wayPkg.readChannel.size(); i++)
        this->addReadSubChannel({wayPkg.readChannel.at(i)});

    for (uint8_t i = 0; i < wayPkg.writeChannel.size(); i++)
        this->addWriteSubChannel({wayPkg.writeChannel.at(i)});

    _timerPing.start(TIMER_PING);
    _reReadTimer.start();

    emit signalResponse_Way();
}

void IOT_Host::response_READ_recived(const IOTV_SH::RESPONSE_PKG &pkg)
{
    if (pkg.type != IOTV_SH::Response_Type::RESPONSE_READ)
        return;

    const IOTV_SH::RESPONSE_READ &readPkg = static_cast<const IOTV_SH::RESPONSE_READ &>(pkg);

    this->setReadChannelData(readPkg.chanelNumber, IOTV_SH::QByteArrayToVecUInt8(readPkg.data));

    if(_logFile.isEmpty())
        return;

    if(this->getReadChannelType(readPkg.chanelNumber) == Raw::DATA_TYPE::STRING)
    {
        Log::write("R:"+ QString::number(readPkg.chanelNumber) + "=" +
                   readPkg.data,
                   Log::Write_Flag::FILE, _logFile);
    }
    else
        Log::write("R:"+ QString::number(readPkg.chanelNumber) + "=" +
                   readPkg.data.toHex(':'),
                   Log::Write_Flag::FILE, _logFile);
}

void IOT_Host::response_WRITE_recived(const IOTV_SH::RESPONSE_PKG &pkg)
{
    //Нет никакой реакции на ответ о записи
    Q_UNUSED(pkg)
}

void IOT_Host::response_PONG_recived(const IOTV_SH::RESPONSE_PKG &pkg)
{
    if (pkg.type != IOTV_SH::Response_Type::RESPONSE_PONG)
        return;

    const IOTV_SH::RESPONSE_PONG &pongPkg = static_cast<const IOTV_SH::RESPONSE_PONG &>(pkg);

    if (pongPkg.state)
    {
        _timerReconnect.start(TIMER_RECONNECT);
        _timerPing.start(TIMER_PING);
    }
}

void IOT_Host::setLogFile(const QString &logFile)
{
    _logFile = logFile;
}

void IOT_Host::setInterval(uint interval)
{
    _reReadTimer.setInterval(interval < 1000 ? 1000 : interval);
}

QString IOT_Host::getLogFile() const
{
    return _logFile;
}

void IOT_Host::slotConnected()
{
    setOnline(true);
    _conn_type->write(IOTV_SH::query_WAY());

    _timerReconnect.start(TIMER_RECONNECT);
    emit signalHostConnected(); ///!!! никуда не идут
}

void IOT_Host::slotDisconnected()
{
    setOnline(false);

    _timerPing.stop();
    _timerReconnect.stop();
    _reReadTimer.stop();


    emit signalHostDisconnected(); ///!!! никуда не идут
}

void IOT_Host::slotReReadTimeOut()
{
    for (int i = 0; i < readChannelLength(); i++)
        readData(i);
}

void IOT_Host::slotPingTimeOut()
{
    //!!!
    QByteArray data;
    data.push_back(IOTV_SH::QUERY_PING_BYTE);
    _conn_type->write(data);
}

void IOT_Host::slotReconnectTimeOut()
{
    _conn_type->disconnectFromHost();
    Log::write(_conn_type->getName() + " WARRNING: ping timeout");
}

void IOT_Host::slotNewThreadStart()
{
    connect(&_timerPing, &QTimer::timeout, this, &IOT_Host::slotPingTimeOut, Qt::QueuedConnection);
    connect(&_timerReconnect, &QTimer::timeout, this, &IOT_Host::slotReconnectTimeOut, Qt::QueuedConnection);
    connect(&_reReadTimer, &QTimer::timeout, this, &IOT_Host::slotReReadTimeOut, Qt::QueuedConnection);

    setInterval(_structSettings.interval);
    setLogFile(_structSettings.logFile);

    if (_structSettings.connection_type == "TCP")
        setConnectionTypeTCP();
    //    else if (_structSettings.connection_type == "COM")
    //    {
    //        COM_conn_type::SetingsPort settingsPort;
    //        settingsPort.baudRate = _settingsHosts.value("baudRate", 115200).toInt();
    //        settingsPort.dataBits = _settingsHosts.value("dataBits", 8).toInt();
    //        settingsPort.parity = _settingsHosts.value("parity", 0).toInt();
    //        settingsPort.stopBits = _settingsHosts.value("stopBits", 1).toInt();
    //        settingsPort.flowControl = _settingsHosts.value("flowControl", 0).toInt();

    //        _iot_hosts.back()->setConnectionTypeCom(address, settingsPort);
    //    }
    else if (_structSettings.connection_type == "FILE")
        setConnectionTypeFile();
    //    else if (_structSettings.connection_type == "UDP")
    //    {
    //        _iot_hosts.back()->setConnectionTypeFile(address);
    //    }
    else
    {
        Log::write("Error: settings file syntax error, [" + _structSettings.name + "]", Log::Write_Flag::FILE_STDERR);
        exit(1);
    }

    connectToHost();
}
