#include "channelmanager.h"

ChannelManager::ChannelManager(QObject *parent) : QObject(parent)
{
    connect(&_timerResponse, &QTimer::timeout, this, &ChannelManager::signalTimerResponse);
}

bool ChannelManager::addReadSubChannel(Raw::DATA_TYPE dataType)
{
    return _readChannel.addSubchannel(dataType);
}

bool ChannelManager::addWriteSubChannel(Raw::DATA_TYPE dataType)
{
    return _writeChannel.addSubchannel(dataType);
}

void ChannelManager::removeAllSubChannel()
{
    _readChannel.removeAllSubchanel();
    _writeChannel.removeAllSubchanel();
}

bool ChannelManager::setReadChannelData(size_t channelNumber, Raw::RAW rawData)
{
    return _readChannel.setData(channelNumber, rawData);
}

Raw::DATA_TYPE ChannelManager::getReadChannelDataType(size_t index) const
{
    return _readChannel.getDataType(index);
}

Raw::RAW ChannelManager::getReadChannelData(size_t index) const
{
    return _readChannel.getData(index);
}

Raw::DATA_TYPE ChannelManager::getWriteChannelDataType(size_t index) const
{
    return _writeChannel.getDataType(index);
}

size_t ChannelManager::readChannelLength() const
{
    return _readChannel.length();
}

size_t ChannelManager::writeChannelLength() const
{
    return _writeChannel.length();
}

bool ChannelManager::insertExpectedResponseRead(uint8_t channelNumber)
{
    if(channelNumber >= _readChannel.length())
    {
        QString strOut = "ERROR: " + QString(Q_FUNC_INFO) + ". channelNumber(" + QString::number(channelNumber)
                         + ") >= Read channel length(" + QString::number(_readChannel.length()) + ")";
        Log::write(strOut, Log::Flags::WRITE_TO_FILE_AND_STDERR);
        return false;
    }

    _expectedResponseRead.insert(channelNumber);
    _timerResponse.start(TIMER_INTERVAL);

    return true;
}

bool ChannelManager::insertExpectedResponseWrite(uint8_t channelNumber, Raw::RAW rawData)
{
    if(channelNumber >= _writeChannel.length())
    {
        QString strOut = "ERROR: " + QString(Q_FUNC_INFO) + ". channelNumber(" + QString::number(channelNumber)
                         + ") >= Write channel length(" + QString::number(_readChannel.length()) + ")";
        Log::write(strOut, Log::Flags::WRITE_TO_FILE_AND_STDERR);

        return false;
    }

    _expectedResponseWrite[channelNumber] = rawData;
    _timerResponse.start(TIMER_INTERVAL);
    return true;
}

void ChannelManager::eraseExpectedResponseRead(uint8_t channelNumber)
{
    _expectedResponseRead.erase(channelNumber);
}

void ChannelManager::eraseExpectedResponseWrite(uint8_t channelNumber)
{
    _expectedResponseWrite.erase(channelNumber);
}

void ChannelManager::eraseAllExpectedResponse()
{
    _expectedResponseRead.clear();
    _expectedResponseWrite.clear();
}

void ChannelManager::stopTimer()
{
    _timerResponse.stop();
}

const std::set<uint8_t> &ChannelManager::getExpectedResponseRead() const
{
    return _expectedResponseRead;
}

const std::map<uint8_t, Raw::RAW> &ChannelManager::getExpectedResponseWrite() const
{
    return _expectedResponseWrite;
}
