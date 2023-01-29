#include "base_host.h"

Base_Host::Base_Host(uint16_t id, QObject *parent) : QObject(parent),
    _id(id), _description("None")
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

bool Base_Host::setReadChannelData(uint8_t channelNumber, const QByteArray &data)
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

QByteArray Base_Host::getReadChannelData(uint8_t channelNumber) const
{
    return _readChannel.getData(channelNumber);
}

Raw::DATA_TYPE Base_Host::getWriteChannelType(uint8_t channelNumber) const
{
    return _writeChannel.getType(channelNumber);
}

uint8_t Base_Host::getReadChannelLength() const
{
    return _readChannel.size();
}

uint8_t Base_Host::getWriteChannelLength() const
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

void Base_Host::setId(uint16_t id)
{
    _id = id;
}

void Base_Host::setDescription(const QString description)
{
    _description = description;
}

void Base_Host::setState(STATE state)
{
    if (_state != state)
        _state = state;
}

Base_Host::STATE Base_Host::state() const
{
    return _state;
}

uint16_t Base_Host::getId() const
{
    return _id;
}

QString Base_Host::getDescription() const
{
    return _description;
}

struct IOTV_Server_embedded *Base_Host::convert() const
{
    auto nameSize = getName().toLocal8Bit().size();
    auto descriptionSize = strlen(getDescription().toStdString().c_str());
    auto numberReadChannel = getReadChannelLength();
    auto numberWriteChannel = getWriteChannelLength();

    struct IOTV_Server_embedded iot = {
        .id = getId(),
        .name = (nameSize > 0) ? (const char * const)malloc(nameSize) : NULL, //!!! нет проверки при не удаче выделить память
        .description = (descriptionSize > 0) ? (const char * const)malloc(descriptionSize) : NULL,
        .numberReadChannel = getReadChannelLength(),
        .readChannel = (numberReadChannel > 0) ? (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * numberReadChannel) : NULL, //!!! нет проверки при не удаче выделить память
        .readChannelType = (numberReadChannel > 0) ? (uint8_t *)malloc(numberReadChannel) : NULL, //!!! нет проверки при не удаче выделить память
        .numberWriteChannel = numberWriteChannel,
        .writeChannelType = (numberWriteChannel > 0) ? (uint8_t *)malloc(numberWriteChannel) : NULL, //!!! нет проверки при не удаче выделить память
        .state = (uint8_t)state()
    };
    //!!! временные данные нормально отработают?
    if (nameSize > 0)
        memcpy((void *)iot.name, getName().toLocal8Bit(), nameSize);
    if (descriptionSize > 0)
        memcpy((void *)iot.description, getDescription().toLocal8Bit(), descriptionSize);

    for (uint8_t i = 0; i < iot.numberReadChannel; ++i)
    {
        iot.readChannel[i].data = (char *)malloc(getReadChannelData(i).size());
        Q_ASSERT(iot.readChannel[i].data != NULL);
        iot.readChannel[i].dataSize = getReadChannelData(i).size();
        memcpy(iot.readChannel[i].data, getReadChannelData(i).data(), iot.readChannel[i].dataSize);
        iot.readChannelType[i] = (uint8_t)getReadChannelType(i);
    }

    for (uint8_t i = 0; i < iot.numberWriteChannel; ++i)
        iot.writeChannelType[i] = (uint8_t)getWriteChannelType(i);

    struct IOTV_Server_embedded *iotResult = (struct IOTV_Server_embedded *)malloc(sizeof(struct IOTV_Server_embedded));
    Q_ASSERT(iotResult != NULL);
    memcpy(iotResult, &iot, sizeof(IOTV_Server_embedded));

    return iotResult;
}
