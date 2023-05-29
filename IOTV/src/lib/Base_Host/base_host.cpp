#include "base_host.h"

Base_Host::Base_Host(uint16_t id, QObject *parent) : QObject(parent),
    _id(id), _description("None"), _state(State_STATE::State_STATE_OFFLINE)
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
    emit signalDataRX(channelNumber, data);
    if (_readChannel.getData(channelNumber) != data)
    {
        _readChannel.setData(channelNumber, data);
        emit signalDataChanged(channelNumber, data);
        return true;
    }

    return false;
}

bool Base_Host::setWriteChannelData(uint8_t channelNumber, const Raw &data)
{
    return _writeChannel.setData(channelNumber, data);
}

Raw::DATA_TYPE Base_Host::getReadChannelType(uint8_t channelNumber) const
{
    return _readChannel.getType(channelNumber);
}

const QByteArray &Base_Host::getReadChannelData(uint8_t channelNumber) const
{
    return _readChannel.getData(channelNumber);
}

const Raw &Base_Host::getReadChannelDataRaw(uint8_t channelNumber) const
{
    return _readChannel.getRawData(channelNumber);
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

void Base_Host::setDescription(const QString &description)
{
    _description = description;
}

void Base_Host::setState(State_STATE newState)
{
    //!!!
    if (newState != State_STATE_ONLINE && newState != State_STATE_OFFLINE)
    {
        emit signalStateUnknow(newState);
        return;
    }

    if (_state != newState)
    {
        _state = newState;
        emit signalStateChanged(_state);

        if (_state == State_STATE_ONLINE)
            emit signalStateOnline();
        else if (_state == State_STATE_OFFLINE)
            emit signalStateOffline();
    }
}

State_STATE Base_Host::state() const
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
    auto descriptionSize = getDescription().toLocal8Bit().size();
    auto numberReadChannel = getReadChannelLength();
    auto numberWriteChannel = getWriteChannelLength();

    struct IOTV_Server_embedded iot = {
        .id = getId(),
        .name = (nameSize > 0) ? static_cast<char *>(malloc(nameSize)) : NULL, //!!! нет проверки при не удаче выделить память
        .description = (descriptionSize > 0) ? static_cast<char *>(malloc(descriptionSize)) : NULL,
        .numberReadChannel = getReadChannelLength(),
        .readChannel = (numberReadChannel > 0) ? static_cast<struct RawEmbedded *>(malloc(sizeof(struct RawEmbedded) * numberReadChannel)) : NULL, //!!! нет проверки при не удаче выделить память
        .readChannelType = (numberReadChannel > 0) ? static_cast<uint8_t *>(malloc(numberReadChannel)) : NULL, //!!! нет проверки при не удаче выделить память
        .numberWriteChannel = numberWriteChannel,
        .writeChannelType = (numberWriteChannel > 0) ? static_cast<uint8_t *>(malloc(numberWriteChannel)) : NULL, //!!! нет проверки при не удаче выделить память
        .state = (uint8_t)state(),
        .nameSize = static_cast<uint8_t>(nameSize),
        .descriptionSize = static_cast<uint16_t>(descriptionSize)
    };

    if (nameSize > 0)
        memcpy(const_cast<char *>(iot.name), getName().toLocal8Bit(), nameSize);
    if (descriptionSize > 0)
        memcpy(const_cast<char *>(iot.description), getDescription().toLocal8Bit(), descriptionSize);

    for (uint8_t i = 0; i < iot.numberReadChannel; ++i)
    {
        auto dataSize = dataSizeonDataType(static_cast<uint8_t>(getReadChannelType(i)));
        auto byteArr = getReadChannelData(i);
        if (numberReadChannel > 0) // что бы не ругался компилятор
        {
            iot.readChannel[i].data = static_cast<char *>(malloc(dataSize));
            Q_ASSERT(iot.readChannel[i].data != NULL);
            iot.readChannel[i].dataSize = dataSize;

            if (byteArr.size() == dataSize)
                memcpy(iot.readChannel[i].data, byteArr.data(), dataSize);
            else
                memset(iot.readChannel[i].data, 0, dataSize);

            iot.readChannelType[i] = (uint8_t)getReadChannelType(i);
        }
    }

    for (uint8_t i = 0; i < iot.numberWriteChannel; ++i)
        iot.writeChannelType[i] = (uint8_t)getWriteChannelType(i);

    struct IOTV_Server_embedded *iotResult = static_cast<struct IOTV_Server_embedded *>(malloc(sizeof(struct IOTV_Server_embedded)));
    Q_ASSERT(iotResult != NULL);
    memcpy(reinterpret_cast<char *>(iotResult), &iot, sizeof(IOTV_Server_embedded));

    return iotResult;
}
