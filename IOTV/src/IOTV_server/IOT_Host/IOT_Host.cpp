#include "IOT_Host.h"
#include "../protocol_class.h"

IOT_Host::IOT_Host(const QString &name, QObject* parent) : QObject(parent),
    _id(0), _description("NO DESCRIPTION"),
    _host(std::make_unique<Base_conn_type>(name)), _logFile(""), _state(0)
{
    connectObjects();
    connect(&channelManager, &ChannelManager::signalTimerResponse, this, &IOT_Host::slotResendData);
}

IOT_Host::~IOT_Host()
{

}

void IOT_Host::printDebugData() const
{
    qDebug() << "\n" << "PRINT DEBUG";
    qDebug() << "name: " + getName();
    qDebug() << "id: " + QString::number(getId());
    qDebug() << "description: " + getDescription();
    qDebug() << "connection type: " + Base_conn_type::ConnTypeToString(getConnectionType());

    qDebug() << "Connection:";
    if(getConnectionType() == Base_conn_type::Conn_type::ETHERNET)
    {
        qDebug() << "\t" << "addr: " << _host->getAddress();

        Base_conn_type *tmpPtr = _host.get();
        qDebug() << "\t" << "port: " + QString::number(qobject_cast<Ethernet_conn_type*>(tmpPtr)->getPort());
    }
    else if(getConnectionType() == Base_conn_type::Conn_type::COM)
        qDebug() << "\t" << "addr: " << _host->getAddress();

    qDebug() << "read channels length: " << channelManager.readChannelLength();
    for (uint8_t i = 0; i < channelManager.readChannelLength(); ++i)
    {
        qDebug() << "\t" << "subchannel #" + QString::number(i);
        qDebug() << "\t" << "data type: " << Raw::toString(channelManager.getReadChannelDataType(i)).c_str();
        qDebug() << "\t" << "data: " << Raw::toString(channelManager.getReadChannelDataType(i), channelManager.getReadChannelData(i)).c_str();
    }

    qDebug() << "write channels length: " << channelManager.writeChannelLength();
    for (uint8_t i = 0; i < channelManager.writeChannelLength(); ++i)
    {
        qDebug() << "\t" << "subchannel #" + QString::number(i);
        qDebug() << "\t" << "data type: " << Raw::toString(channelManager.getWriteChannelDataType(i)).c_str();
    }

    qDebug() << "\n";
}

Base_conn_type::Conn_type IOT_Host::getConnectionType() const
{
    return _host->getConnectionType();
}

qint64 IOT_Host::readData(uint8_t channelNumber)
{
    if(!_state.testFlag(Flag::DeviceRegistered) || _state.testFlag(Flag::ExpectedWay))
        return -1;

    QByteArray data;
    Protocol_Class::query_READ(data, channelNumber);

    if(channelManager.insertExpectedResponseRead(channelNumber))
        return _host->write(data);

    return -1;
}

qint64 IOT_Host::writeData(uint8_t channelNumber, Raw::RAW rawData)
{
    if(!_state.testFlag(Flag::DeviceRegistered) || _state.testFlag(Flag::ExpectedWay))
        return -1;

    QByteArray data;
    Protocol_Class::query_WRITE(data, channelNumber, rawData);

    if(channelManager.insertExpectedResponseWrite(channelNumber, rawData))
        return _host->write(data);

    return -1;
}

QString IOT_Host::getName() const
{
    return _host.get()->getName();
}

uint8_t IOT_Host::getId() const
{
    return _id;
}

QString IOT_Host::getDescription() const
{
    return _description;
}

void IOT_Host::setConnectionTypeEthernet(const QString &addr, quint16 port)
{
    _host = std::make_unique<Ethernet_conn_type>(_host.get()->getName());
    qobject_cast<Ethernet_conn_type*>(_host.get())->setAddress(addr);
    qobject_cast<Ethernet_conn_type*>(_host.get())->setPort(port);

    connectObjects();
}

void IOT_Host::setConnectionTypeCom(const QString &addr)
{
    _host = std::make_unique<COM_conn_type>(_host.get()->getName());
    qobject_cast<COM_conn_type*>(_host.get())->setAddress(addr);

    connectObjects();
}

void IOT_Host::setId(uint8_t id)
{
    _id = id;
}

void IOT_Host::setDescription(const QString &description)
{
    _description = description;
}

void IOT_Host::connectToHost()
{
    _host->connectToHost();
}

void IOT_Host::connectObjects() const
{
    connect(_host.get(), &Base_conn_type::signalConnected, this, &IOT_Host::slotConnected);
    connect(_host.get(), &Base_conn_type::signalDisconnected, this, &IOT_Host::slotDisconnected);
    connect(_host.get(), &Base_conn_type::signalDataRiceved, this, &IOT_Host::slotDataRiceved);
}

void IOT_Host::response_WAY_recived(const QByteArray &data)
{
    if(!_state.testFlag(Flag::ExpectedWay))
    {
        Log::write(_host->getName() + "WARNING: Received a packet RESPONSE_WAY without a request.");
        return;
    }

    Protocol_Class::response_WAY(this, data);
    _state.setFlag(DeviceRegistered);
    _state.setFlag(ExpectedWay, false);
}

void IOT_Host::response_READ_recived(const QByteArray &data)
{
    if(_state.testFlag(Flag::ExpectedWay) || !_state.testFlag(Flag::DeviceRegistered))
    {
        Log::write(_host->getName() + "WARNING: The device is not registered or initialized, but a packet RESPONSE_READ is received.");
        return;
    }

    auto list = channelManager.getExpectedResponseRead();
    auto it = list.find(Protocol_Class::channelNumber(data[0]));
    if(it == list.end())
        return;

    Protocol_Class::response_READ(this, data);
    if(!_logFile.isEmpty())
    {
        uint8_t channelNumber = Protocol_Class::channelNumber(data[0]);
        Raw::DATA_TYPE dt = channelManager.getReadChannelDataType(channelNumber);
        Raw::RAW raw = channelManager.getReadChannelData(channelNumber);

        Log::write("R:"+ QString::number(channelNumber) + "=" +
                   Raw::toString(dt, raw).c_str(),
                   Log::Flags::WRITE_TO_FILE_ONLY, _logFile);
    }
}

void IOT_Host::response_WRITE_recived(const QByteArray &data)
{
    if(_state.testFlag(Flag::ExpectedWay) || !_state.testFlag(Flag::DeviceRegistered))
    {
        Log::write(_host->getName() + "WARRNING: The device is not registered or initialized, but a packet RESPONSE_WRITE is received.");
        return;
    }
    Protocol_Class::response_WRITE(this, data);
}

void IOT_Host::setLogFile(const QString &logFile)
{
    _logFile = logFile;
}

void IOT_Host::setInterval(uint interval)
{
    disconnect(&_intervalTimer, &QTimer::timeout, this, &IOT_Host::slotTimeOut);
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
    Protocol_Class::query_WAY(data);
    _state.setFlag(ExpectedWay);
    _host->write(data);

    emit signalHostConnected();
}

void IOT_Host::slotDisconnected()
{
    _state.setFlag(DeviceRegistered, false);
    channelManager.eraseAllExpectedResponse();

    emit signalHostDisconnected();
}

void IOT_Host::slotDataRiceved(QByteArray data)
{   
    for (auto &packetData : Protocol_Class::splitResponseData(data))
    {
        Protocol_Class::Response_Type dataType = Protocol_Class::checkResponsetData(packetData);

        if(dataType == Protocol_Class::Response_Type::RESPONSE_WAY)
        {
            response_WAY_recived(packetData);
            readData(0);
            readData(1);
        }
        else if(dataType == Protocol_Class::Response_Type::RESPONSE_READ)
            response_READ_recived(packetData);
        else if(dataType == Protocol_Class::Response_Type::RESPONSE_WRITE)
            response_WRITE_recived(packetData);
        else
            Log::write(_host->getName() + " WARRNING: received data UNKNOW: " + packetData.toHex(':'));

        emit signalDataRiceved();
    }
}

void IOT_Host::slotResendData()
{
    channelManager.stopTimer();

    const std::set<uint8_t> expectedResponseRead = channelManager.getExpectedResponseRead();
    for (uint8_t channelNumber : expectedResponseRead)
        readData(channelNumber);

    const std::map<uint8_t, Raw::RAW> &expectedResponseWrite = channelManager.getExpectedResponseWrite();
    for (auto [key, value] : expectedResponseWrite)
        writeData(key, value);
}

void IOT_Host::slotTimeOut()
{
    for (size_t i = 0; i <  channelManager.readChannelLength(); i++)
        readData(i);
}

