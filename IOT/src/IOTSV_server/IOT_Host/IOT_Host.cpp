#include "IOT_Host.h"
#include "../protocol_class.h"

IOT_Host::IOT_Host(const QString &name, QObject* parent) : QObject(parent),
    _id(0), _description("NO DESCRIPTION"), _userDescription("NO DESCRIPTION"),
     _host(std::make_unique<Base_conn_type>(name)), _state(0)
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
    qDebug() << "user description: " + getUser_description();
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

QString IOT_Host::getUser_description() const
{
    return _userDescription;
}

void IOT_Host::setConnectionType(Base_conn_type::Conn_type conn_type)
{
    if(conn_type == Base_conn_type::Conn_type::ETHERNET)
        _host = std::make_unique<Ethernet_conn_type>(_host.get()->getName());
    else if(conn_type == Base_conn_type::Conn_type::COM)
        _host = std::make_unique<COM_conn_type>(_host.get()->getName());

    connectObjects();
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

void IOT_Host::setUser_description(const QString &userDescription)
{
    _userDescription = userDescription;
}

void IOT_Host::setName(const QString &name)
{
    _host.get()->setName(name);
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

void IOT_Host::slotConnected()
{
    QByteArray data;
    Protocol_Class::query_WAY(data);
    _state.setFlag(ExpectedWay);
    _host->write(data);
}

void IOT_Host::slotDisconnected()
{
    _state.setFlag(DeviceRegistered, false);
    channelManager.eraseAllExpectedResponse();
}

void IOT_Host::slotDataRiceved(QByteArray data)
{   
    Protocol_Class::Response_Type dataType = Protocol_Class::checkResponsetData(data);

    if(dataType == Protocol_Class::Response_Type::RESPONSE_WAY)
    {
        if(!_state.testFlag(Flag::ExpectedWay))
        {
            Log::write(_host->getName() + "WARNING: Received a packet RESPONSE_WAY without a request.");
            return;
        }
        Protocol_Class::response_WAY(this, data);
        _state.setFlag(DeviceRegistered);
        _state.setFlag(ExpectedWay, false);

//        readData(0);
    }
    else if(dataType == Protocol_Class::Response_Type::RESPONSE_READ)
    {
        if(_state.testFlag(Flag::ExpectedWay) || !_state.testFlag(Flag::DeviceRegistered))
        {
            Log::write(_host->getName() + "WARNING: The device is not registered or initialized, but a packet RESPONSE_READ is received.");
            return;
        }
        Protocol_Class::response_READ(this, data);
    }
    else if(dataType == Protocol_Class::Response_Type::RESPONSE_WRITE)
    {
        if(_state.testFlag(Flag::ExpectedWay) || !_state.testFlag(Flag::DeviceRegistered))
        {
            Log::write(_host->getName() + "WARRNING: The device is not registered or initialized, but a packet RESPONSE_WRITE is received.");
            return;
        }
        Protocol_Class::response_WRITE(this, data);
    }
    else
        Log::write(_host->getName() + "WARRNING: received data UNKNOW: " + data.toHex(':'));
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

void IOT_Host::slotShedule()
{
    Shedule* shedule = qobject_cast<Shedule*>(sender());

    if(shedule->RW == 'r')
    {
        readData(shedule->channelNumber);
    }
    else if(shedule->RW == 'w')
    {
//        writeData(shedule->channelNumber);
    }
}
