#include "../Protocols/IOTV_SH.h"

#include "IOT_Host.h"

IOT_Host::IOT_Host(const QString &name, QObject* parent) : Base_Host(parent),
    _conn_type(std::make_unique<Base_conn_type>(name)), _logFile(""), _state(0)
{
    connectObjects();
    connect(this, &Base_Host::signalTimerResponse, this, &IOT_Host::slotResendData);
}

void IOT_Host::printDebugData() const
{
    qDebug() << "\n" << "PRINT DEBUG";
    qDebug() << "name: " << getName();
    qDebug() << "id: " + QString::number(getId());
    qDebug() << "description: " << getDescription();
    qDebug() << "connection type: " + Base_conn_type::ConnTypeToString(getConnectionType());

    qDebug() << "Connection:";
    if(getConnectionType() == Base_conn_type::Conn_type::ETHERNET)
    {
        qDebug() << "\t" << "addr: " << _conn_type->getAddress();

        Base_conn_type *tmpPtr = _conn_type.get();
        qDebug() << "\t" << "port: " + QString::number(qobject_cast<Ethernet_conn_type*>(tmpPtr)->getPort());
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
    if(state)
    {
        _state.setFlag(DeviceRegistered);
        _state.setFlag(ExpectedWay, false);
    }
    else
    {
        _state.setFlag(DeviceRegistered, false);
        _state.setFlag(ExpectedWay, false);
    }
}

bool IOT_Host::getState() const
{
    return _state.testFlag(Flag::DeviceRegistered);
}

int64_t IOT_Host::readData(uint8_t channelNumber)
{
    if(!_state.testFlag(Flag::DeviceRegistered) || _state.testFlag(Flag::ExpectedWay))
        return -1;

    QByteArray data;
    IOTV_SH::query_READ(data, channelNumber);

    if(insertExpectedResponseRead(channelNumber))
        return _conn_type->write(data);

    return -1;
}

int64_t IOT_Host::writeData(uint8_t channelNumber, Raw::RAW rawData)
{
    if(!_state.testFlag(Flag::DeviceRegistered) || _state.testFlag(Flag::ExpectedWay))
        return -1;

    QByteArray data;
    IOTV_SH::query_WRITE(data, channelNumber, rawData);

    if(insertExpectedResponseWrite(channelNumber, rawData))
        return _conn_type->write(data);

    return -1;
}

void IOT_Host::dataResived(QByteArray data)
{
    for (auto &packetData : IOTV_SH::splitResponseData(data))
    {
        IOTV_SH::Response_Type dataType = IOTV_SH::checkResponsetData(packetData);

        if(dataType == IOTV_SH::Response_Type::RESPONSE_WAY)
            response_WAY_recived(packetData);
        else if(dataType == IOTV_SH::Response_Type::RESPONSE_READ)
            response_READ_recived(packetData);
        else if(dataType == IOTV_SH::Response_Type::RESPONSE_WRITE)
            response_WRITE_recived(packetData);
        else
            Log::write(_conn_type->getName() + " WARRNING: received data UNKNOW: " + packetData.toHex(':'));

        emit signalDataRiceved(); // !!!
    }
}

QString IOT_Host::getName() const
{
    return _conn_type.get()->getName();
}

void IOT_Host::setConnectionTypeEthernet(const QString &addr, quint16 port)
{
    _conn_type = std::make_unique<Ethernet_conn_type>(_conn_type.get()->getName());
    qobject_cast<Ethernet_conn_type*>(_conn_type.get())->setAddress(addr);
    qobject_cast<Ethernet_conn_type*>(_conn_type.get())->setPort(port);

    connectObjects();
}

void IOT_Host::setConnectionTypeCom(const QString &addr)
{
    _conn_type = std::make_unique<COM_conn_type>(_conn_type.get()->getName());
    qobject_cast<COM_conn_type*>(_conn_type.get())->setAddress(addr);

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
    if(!_state.testFlag(Flag::ExpectedWay))
    {
        Log::write(_conn_type->getName() + "WARNING: Received a packet RESPONSE_WAY without a request.");
        return;
    }

    IOTV_SH::response_WAY(*this, data);

    setState(true);
}

void IOT_Host::response_READ_recived(const QByteArray &data)
{
    if(_state.testFlag(Flag::ExpectedWay) || !_state.testFlag(Flag::DeviceRegistered))
    {
        Log::write(_conn_type->getName() + "WARNING: The device is not registered or initialized, but a packet RESPONSE_READ is received.");
        return;
    }

    auto list = getExpectedResponseRead();
    auto it = list.find(IOTV_SH::channelNumber(data[0]));
    if(it == list.end())
        return;

    IOTV_SH::response_READ(*this, data);
    if(!_logFile.isEmpty())
    {
        uint8_t channelNumber = IOTV_SH::channelNumber(data[0]);
        Raw::DATA_TYPE dt = getReadChannelDataType(channelNumber);
        Raw::RAW raw = getReadChannelData(channelNumber);

        Log::write("R:"+ QString::number(channelNumber) + "=" +
                   Raw::toString(dt, raw).c_str(),
                   Log::Flags::WRITE_TO_FILE_ONLY, _logFile);
    }
}

void IOT_Host::response_WRITE_recived(const QByteArray &data)
{
    if(_state.testFlag(Flag::ExpectedWay) || !_state.testFlag(Flag::DeviceRegistered))
    {
        Log::write(_conn_type->getName() + "WARRNING: The device is not registered or initialized, but a packet RESPONSE_WRITE is received.");
        return;
    }
    IOTV_SH::response_WRITE(*this, data);
}

void IOT_Host::setLogFile(const QString &logFile)
{
    _logFile = logFile;
}

void IOT_Host::setInterval(uint interval)
{
    _intervalTimer.stop();
//    disconnect(&_intervalTimer, &QTimer::timeout, this, &IOT_Host::slotTimeOut);
    if(interval)
    {
        connect(&_intervalTimer, &QTimer::timeout, this, &IOT_Host::slotTimeOut);
        _intervalTimer.start(interval);
    }
}

QString IOT_Host::getLogFile() const
{
    return _logFile;
}

void IOT_Host::slotConnected()
{
    QByteArray data;
    IOTV_SH::query_WAY(data);
    _state.setFlag(ExpectedWay);
    _conn_type->write(data);

    emit signalHostConnected();
}

void IOT_Host::slotDisconnected()
{
    setState(false);
    eraseAllExpectedResponse();

    emit signalHostDisconnected();
}

void IOT_Host::slotResendData()
{
    stopTimer();

    const std::set<uint8_t> expectedResponseRead = getExpectedResponseRead();
    for (uint8_t channelNumber : expectedResponseRead)
        readData(channelNumber);

    const std::map<uint8_t, Raw::RAW> &expectedResponseWrite = getExpectedResponseWrite();
    for (auto [key, value] : expectedResponseWrite)
        writeData(key, value);
}

void IOT_Host::slotTimeOut()
{
    for (size_t i = 0; i <  readChannelLength(); i++)
        readData(i);
}

