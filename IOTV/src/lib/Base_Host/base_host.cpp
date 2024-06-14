#include "base_host.h"

#include "iotv_server_embedded.h"

Base_Host::Base_Host(uint16_t id, QObject *parent) : QObject(parent),
    _id(id), _description("None"), _state(state_t::State_STATE_OFFLINE)
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

bool Base_Host::addReadChannelData(uint8_t channelNumber, const QByteArray &data)
{
    return _readChannel.addData(channelNumber, {data.data(), data.size()});
}

bool Base_Host::setReadChannelData(uint8_t channelNumber, const QByteArray &data)
{
    bool result = false;

    if (_readChannel.getData(channelNumber) != data)
    {
        result = true;
        _readChannel.setData(channelNumber, data);
        emit signalDataChanged(channelNumber, data);
    }
    emit signalDataRX(channelNumber, data);

    return result;
}

void Base_Host::clearReadChannelData(uint8_t channelNumber)
{
    _readChannel.clearData(channelNumber);
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

void Base_Host::setState(state_t newState)
{
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

state_t Base_Host::state() const
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

iotv_obj_t *Base_Host::convert() const
{
    uint8_t nameSize = strlen(getName().toStdString().c_str());
    uint16_t descriptionSize = strlen(getDescription().toStdString().c_str());
    auto numberReadChannel = getReadChannelLength();
    auto numberWriteChannel = getWriteChannelLength();

    iotv_obj_t *iot = static_cast<iotv_obj_t *>(malloc(sizeof(iotv_obj_t)));
    if (iot == nullptr)
        return nullptr;

    iot->id = getId();
    iot->numberReadChannel = getReadChannelLength();
    iot->numberWriteChannel = numberWriteChannel;
    iot->state = (uint8_t)state();
    iot->nameSize = nameSize;
    iot->descriptionSize = descriptionSize;
    iot->readChannel = (numberReadChannel > 0) ? static_cast<struct RawEmbedded *>(malloc(sizeof(struct RawEmbedded) * numberReadChannel)) : NULL; //!!! нет проверки при не удаче выделить память
    iot->readChannelType = (numberReadChannel > 0) ? static_cast<uint8_t *>(malloc(numberReadChannel)) : NULL; //!!! нет проверки при не удаче выделить память
    iot->writeChannelType = (numberWriteChannel > 0) ? static_cast<uint8_t *>(malloc(numberWriteChannel)) : NULL; //!!! нет проверки при не удаче выделить память
    iot->name = (nameSize > 0) ? static_cast<char *>(malloc(nameSize)) : NULL; //!!! нет проверки при не удаче выделить память
    iot->description = (descriptionSize > 0) ? static_cast<char *>(malloc(descriptionSize)) : NULL;

    //!!! сделать проверку на выделение памяти в  iot

    if (nameSize > 0 && iot->name != NULL)
        memcpy(const_cast<char *>(iot->name), getName().toStdString().c_str(), nameSize);
    if (descriptionSize > 0 && iot->description != NULL)
        memcpy(const_cast<char *>(iot->description), getDescription().toStdString().c_str(), descriptionSize);

    for (uint8_t i = 0; i < iot->numberReadChannel; ++i)
    {
//        if (i == 14)
//        {
//            qDebug() << getReadChannelData(14).size();
//        }

        auto t = getReadChannelType(i);
        uint32_t dataSize = 0;

        if (t == Raw::DATA_TYPE::STRING)
            dataSize = _readChannel.getData(i).size();
        else
            dataSize = dataSizeonDataType(static_cast<uint8_t>(t));

        auto byteArr = getReadChannelData(i);
        if (numberReadChannel > 0 /*&& dataSize > 0*/)
        {
            if (dataSize > 0)
                iot->readChannel[i].data = static_cast<char *>(malloc(dataSize));
            else
                iot->readChannel[i].data = nullptr;

            if (iot->readChannel[i].data == nullptr)
                dataSize = 0;

            iot->readChannel[i].dataSize = dataSize;

            if (dataSize > 0)
            {
                if (byteArr.size() == dataSize)
                    memcpy(iot->readChannel[i].data, byteArr.data(), dataSize);
                else
                    memset(iot->readChannel[i].data, 0, dataSize);
            }

            iot->readChannelType[i] = static_cast<uint8_t>(t);
        }
    }

    for (uint8_t i = 0; i < iot->numberWriteChannel; ++i)
        iot->writeChannelType[i] = (uint8_t)getWriteChannelType(i);

    return iot;
}
