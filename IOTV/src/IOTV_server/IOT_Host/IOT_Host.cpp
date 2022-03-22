#include "../Protocols/IOTV_SH.h"

#include "IOT_Host.h"

IOT_Host::IOT_Host(const QString &name, QObject* parent) : Base_Host(0, parent),
    _conn_type(std::make_unique<Base_conn_type>(name)), _logFile("")
{
//    connectObjects();

    connect(&_timerPing, &QTimer::timeout, this, &IOT_Host::slotPingTimeOut);
    connect(&_timerReconnect, &QTimer::timeout, this, &IOT_Host::slotReconnectTimeOut);
    connect(&_reReadTimer, &QTimer::timeout, this, &IOT_Host::slotReReadTimeOut);
}

void IOT_Host::printDebugData() const
{
    qDebug() << "\n" << "PRINT DEBUG";
    qDebug() << "name: " << getName();
    qDebug() << "id: " + QString::number(getId());
    qDebug() << "description: " << getDescription();
    qDebug() << "connection type: " + Base_conn_type::ConnTypeToString(getConnectionType());

    qDebug() << "Connection:";
    if(getConnectionType() == Base_conn_type::Conn_type::TCP)
    {
        qDebug() << "\t" << "addr: " << _conn_type->getAddress();

        Base_conn_type *tmpPtr = _conn_type.get();
        qDebug() << "\t" << "port: " + QString::number(qobject_cast<TCP_conn_type*>(tmpPtr)->getPort());
    }
    else if(getConnectionType() == Base_conn_type::Conn_type::COM)
        qDebug() << "\t" << "addr: " << _conn_type->getAddress();

    qDebug() << "read channels length: " << readChannelLength();
    for (uint8_t i = 0; i < readChannelLength(); ++i)
    {
        qDebug() << "\t" << "subchannel #" + QString::number(i);
        qDebug() << "\t" << "data type: " << Raw::toString(getReadChannelDataType(i)).c_str();
        qDebug() << "\t" << "data: " << Raw::toString(getReadChannelDataType(i), getReadChannelData(i)).c_str();
    }

    qDebug() << "write channels length: " << writeChannelLength();
    for (uint8_t i = 0; i < writeChannelLength(); ++i)
    {
        qDebug() << "\t" << "subchannel #" + QString::number(i);
        qDebug() << "\t" << "data type: " << Raw::toString(getWriteChannelDataType(i)).c_str();
    }

    qDebug() << "\n";
}

Base_conn_type::Conn_type IOT_Host::getConnectionType() const
{
    return _conn_type->getConnectionType();
}

void IOT_Host::setState(bool state)
{
    _state_flags.setFlag(DeviceRegistered, state);
}

bool IOT_Host::getState() const
{
    return _state_flags.testFlag(Flag::DeviceRegistered);
}

qint64 IOT_Host::readData(uint8_t channelNumber)
{
    if(!_state_flags.testFlag(Flag::DeviceRegistered))// || _state.testFlag(Flag::ExpectedWay))
        return -1;

    QByteArray data = IOTV_SH::query_READ(channelNumber);
    return  writeToServer(data);
}

qint64 IOT_Host::writeData(uint8_t channelNumber, Raw::RAW &rawData)
{
    if(!_state_flags.testFlag(Flag::DeviceRegistered))// || _state.testFlag(Flag::ExpectedWay))
        return -1;

    return IOTV_SH::query_WRITE(*this, channelNumber, rawData);
}

qint64 IOT_Host::writeToServer(QByteArray &data)
{
    return _conn_type->write(data);
}

void IOT_Host::dataResived(QByteArray data)
{
    IOTV_SH::Response_Type dataType = IOTV_SH::checkResponsetData(data);

    if(!_state_flags.testFlag(Flag::DeviceRegistered) && dataType != IOTV_SH::Response_Type::RESPONSE_WAY)
    {
        Log::write(_conn_type->getName() + " WARRNING: received data but device is not registered: " + data.toHex(':'));
        return;
    }

    if(dataType == IOTV_SH::Response_Type::RESPONSE_WAY)
        response_WAY_recived(data);
    else if(dataType == IOTV_SH::Response_Type::RESPONSE_PONG)
        response_PONG_recived();
    else if(dataType == IOTV_SH::Response_Type::RESPONSE_READ)
        response_READ_recived(data);
    else if(dataType == IOTV_SH::Response_Type::RESPONSE_WRITE)
        response_WRITE_recived(data);
    else
    {
        Log::write(_conn_type->getName() + " WARRNING: received data UNKNOW: " + data.toHex(':'));
        _conn_type->disconnectFromHost();
    }
}

QString IOT_Host::getName() const
{
    return _conn_type.get()->getName();
}

void IOT_Host::setConnectionTypeTCP(const QString &addr, quint16 port)
{
    _conn_type = std::make_unique<TCP_conn_type>(_conn_type.get()->getName());

    TCP_conn_type* eth = qobject_cast<TCP_conn_type*>(_conn_type.get());
    eth->setAddress(addr);
    eth->setPort(port);

    connectObjects();
}

void IOT_Host::setConnectionTypeCom(const QString &addr, const COM_conn_type::SetingsPort &settingPort)
{
    _conn_type = std::make_unique<COM_conn_type>(_conn_type.get()->getName());

    COM_conn_type *com = qobject_cast<COM_conn_type*>(_conn_type.get());
    com->setAddress(addr);
    com->setSettingsPort(settingPort);

    connectObjects();
}

void IOT_Host::setConnectionTypeFile(const QString &addr)
{
    _conn_type = std::make_unique<File_conn_type>(_conn_type.get()->getName(), addr);

    connectObjects();
}


void IOT_Host::connectToHost()
{
    _conn_type->connectToHost();
}

void IOT_Host::connectObjects() const
{
    connect(_conn_type.get(), &Base_conn_type::signalConnected, this, &IOT_Host::slotConnected);
    connect(_conn_type.get(), &Base_conn_type::signalDisconnected, this, &IOT_Host::slotDisconnected);
    connect(_conn_type.get(), &Base_conn_type::signalDataRiceved, this, &IOT_Host::dataResived);
}

void IOT_Host::response_WAY_recived(const QByteArray &data)
{
    IOTV_SH::response_WAY(*this, data);

    setState(true);
    _timerPing.start(TIMER_PING);
    _reReadTimer.start();
}

void IOT_Host::response_READ_recived(const QByteArray &data)
{
    IOTV_SH::response_READ(*this, data);
    if(!_logFile.isEmpty())
    {
        uint8_t channelNumber = IOTV_SH::channelNumber(data[0]);
        Raw::DATA_TYPE dt = getReadChannelDataType(channelNumber);
        Raw::RAW raw = getReadChannelData(channelNumber);

        if(dt == Raw::DATA_TYPE::CHAR_PTR)
        {
            Log::write("R:"+ QString::number(channelNumber) + "=" +
                       raw.str,
                       Log::Flags::WRITE_TO_FILE_ONLY, _logFile);
        }
        else
            Log::write("R:"+ QString::number(channelNumber) + "=" +
                       Raw::toString(dt, raw).c_str(),
                       Log::Flags::WRITE_TO_FILE_ONLY, _logFile);
    }
}

void IOT_Host::response_WRITE_recived(const QByteArray &data)
{
    IOTV_SH::response_WRITE(*this, data);
}

void IOT_Host::response_PONG_recived()
{
    _timerReconnect.start(TIMER_RECONNECT);
    _timerPing.start(TIMER_PING);
}

void IOT_Host::setLogFile(const QString &logFile)
{
    _logFile = logFile;
}

void IOT_Host::setInterval(uint interval)
{
    _reReadTimer.setInterval(interval);
}

QString IOT_Host::getLogFile() const
{
    return _logFile;
}

void IOT_Host::slotConnected()
{
    _conn_type->write(IOTV_SH::query_WAY());

    _timerReconnect.start(TIMER_RECONNECT);
    emit signalHostConnected(); ///!!! никуда не идут
}

void IOT_Host::slotDisconnected()
{
    setState(false);
    _timerPing.stop();
    _reReadTimer.stop();
    _timerReconnect.stop();

    emit signalHostDisconnected(); ///!!! никуда не идут
}

void IOT_Host::slotReReadTimeOut()
{
    for (int i = 0; i < readChannelLength(); i++)
        readData(i);
}

void IOT_Host::slotPingTimeOut()
{
    QByteArray data;
    data.push_back(QUERY_PING_BYTE);
    _conn_type->write(data);
}

void IOT_Host::slotReconnectTimeOut()
{
    _conn_type->disconnectFromHost();
    Log::write(_conn_type->getName() + " WARRNING: ping timeout");
}

