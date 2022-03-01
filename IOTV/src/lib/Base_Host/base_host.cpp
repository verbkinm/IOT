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

int Base_Host::readChannelLength() const
{
    return _readChannel.length();
}

int Base_Host::writeChannelLength() const
{
    return _writeChannel.length();
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
