#include "base_host.h"

Base_Host::Base_Host(uint8_t id, QObject *parent) : QObject(parent),
    _id(id), _description("None description")
{

}

void Base_Host::addWriteSubChannel(const Raw &data)
{
    _writeChannel.addSubchannel(data);
}

void Base_Host::addReadSubChannel(const Raw &data)
{
    return _readChannel.addSubchannel(data);
}

void Base_Host::removeAllSubChannel()
{
    _readChannel.removeAllSubchanel();
    _writeChannel.removeAllSubchanel();
}

bool Base_Host::setReadChannelData(uint8_t channelNumber, const Raw &data)
{
    return _readChannel.setData(channelNumber, data);
}

bool Base_Host::setReadChannelData(uint8_t channelNumber, const std::vector<uint8_t> &data)
{
    return _readChannel.setData(channelNumber, data);
}

bool Base_Host::setWriteChannelData(uint8_t channelNumber, const Raw &data)
{
    return _writeChannel.setData(channelNumber, data);
}

Raw::DATA_TYPE Base_Host::getReadChannelType(uint8_t channelNumber) const
{
    return _readChannel.getType(channelNumber);
}

Raw Base_Host::getReadChannelData(uint8_t channelNumber) const
{
    return _readChannel.getData(channelNumber);
}

Raw::DATA_TYPE Base_Host::getWriteChannelType(uint8_t channelNumber) const
{
    return _writeChannel.getType(channelNumber);
}

uint8_t Base_Host::readChannelLength() const
{
    return _readChannel.size();
}

uint8_t Base_Host::writeChannelLength() const
{
    return _writeChannel.size();
}

void Base_Host::setWriteChannel(const Channel &newWriteChannel)
{
    _writeChannel = newWriteChannel;
}

void Base_Host::setReadChannel(const Channel &newReadChannel)
{
    _readChannel = newReadChannel;
}

void Base_Host::setId(uint8_t id)
{
    _id = id;
}

void Base_Host::setDescription(const std::string &description)
{
    _description = description;
}

uint8_t Base_Host::getId() const
{
    return _id;;
}

std::string Base_Host::getDescription() const
{
    return _description;
}
