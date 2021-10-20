#include "base_host.h"

Base_Host::Base_Host(uint8_t id, QObject *parent) : QObject(parent), _id(id), _description("None description")
{

}

Base_Host::~Base_Host()
{

}

bool Base_Host::addReadSubChannel(Raw::DATA_TYPE dataType)
{
    return _readChannel.addSubchannel(dataType);
}

bool Base_Host::addWriteSubChannel(Raw::DATA_TYPE dataType)
{
    return _writeChannel.addSubchannel(dataType);
}

void Base_Host::removeAllSubChannel()
{
    _readChannel.removeAllSubchanel();
    _writeChannel.removeAllSubchanel();
}

bool Base_Host::setReadChannelData(size_t channelNumber, Raw::RAW rawData)
{
    return _readChannel.setData(channelNumber, rawData);
}

Raw::DATA_TYPE Base_Host::getReadChannelDataType(uint8_t channelNumber) const
{
    return _readChannel.getDataType(channelNumber);
}

Raw::RAW Base_Host::getReadChannelData(uint8_t channelNumber) const
{
    return _readChannel.getData(channelNumber);
}

Raw::DATA_TYPE Base_Host::getWriteChannelDataType(uint8_t channelNumber) const
{
    return _writeChannel.getDataType(channelNumber);
}

size_t Base_Host::readChannelLength() const
{
    return _readChannel.length();
}

size_t Base_Host::writeChannelLength() const
{
    return _writeChannel.length();
}

bool Base_Host::insertExpectedResponseRead(uint8_t channelNumber)
{
    if(channelNumber >= _readChannel.length())
    {
        QString strOut = "ERROR: " + QString(Q_FUNC_INFO) + ". channelNumber(" + QString::number(channelNumber)
                         + ") >= Read channel length(" + QString::number(_readChannel.length()) + ")";
        Log::write(strOut, Log::Flags::WRITE_TO_FILE_AND_STDERR);
        return false;
    }

    _expectedResponseRead.insert(channelNumber);
    _timerResponseRead.start(TIMER_INTERVAL);

    return true;
}

bool Base_Host::insertExpectedResponseWrite(uint8_t channelNumber, Raw::RAW rawData)
{
    if(channelNumber >= _writeChannel.length())
    {
        QString strOut = "ERROR: " + QString(Q_FUNC_INFO) + ". channelNumber(" + QString::number(channelNumber)
                         + ") >= Write channel length(" + QString::number(_readChannel.length()) + ")";
        Log::write(strOut, Log::Flags::WRITE_TO_FILE_AND_STDERR);

        return false;
    }

    _expectedResponseWrite[channelNumber] = rawData;
    _timerResponseWrite.start(TIMER_INTERVAL);
    return true;
}

void Base_Host::eraseExpectedResponseRead(uint8_t channelNumber)
{
    _expectedResponseRead.erase(channelNumber);
}

void Base_Host::eraseExpectedResponseWrite(uint8_t channelNumber)
{
    _expectedResponseWrite.erase(channelNumber);
}

void Base_Host::eraseAllExpectedResponse()
{
    _expectedResponseRead.clear();
    _expectedResponseWrite.clear();
}

//void Base_Host::stopTimerWrite()
//{
//    _timerResponseWrite.stop();
//}

//void Base_Host::stopTimerRead()
//{
//    _timerResponseRead.stop();
//}

const std::set<uint8_t> &Base_Host::getExpectedResponseRead() const
{
    return _expectedResponseRead;
}

const std::map<uint8_t, Raw::RAW> &Base_Host::getExpectedResponseWrite() const
{
    return _expectedResponseWrite;
}

void Base_Host::setId(uint8_t id)
{
    _id = id;
}

void Base_Host::setDescription(const QString &description)
{
    _description = description;
}

uint8_t Base_Host::getId() const
{
    return _id;;
}

QString Base_Host::getDescription() const
{
    return _description;
}
