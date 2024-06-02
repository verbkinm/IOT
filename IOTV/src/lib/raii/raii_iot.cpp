#include "raii_iot.h"
#include <cstring>

RAII_iot::RAII_iot(iotv_obj_t *iot)
{
    if (iot == nullptr)
    {
        _iot = nullptr;
        return;
    }

    _iot = iot;

    uint8_t readChannelNumber = _iot->numberReadChannel;

    // readChannel
    if ((_iot->readChannel != nullptr) && (readChannelNumber > 0))
    {
        _iot->readChannel = static_cast<raw_embedded_t *>(malloc(sizeof(raw_embedded_t) * readChannelNumber));
        if (_iot->readChannel != nullptr)
        {
            std::memcpy(_iot->readChannel, iot->readChannel, readChannelNumber);

            for (uint8_t i = 0; i < readChannelNumber; i++)
            {
                raw_embedded_t *cur_readChannel = &_iot->readChannel[i];
                if ((cur_readChannel->data != nullptr) && (cur_readChannel->dataSize > 0))
                {
                    cur_readChannel->data = static_cast<char *>(malloc(cur_readChannel->dataSize));
                    if (cur_readChannel->data != nullptr)
                        std::memcpy(cur_readChannel->data, iot->readChannel[i].data, cur_readChannel->dataSize);
                }
            }
        }
    }

    // readChannelType
    if ((_iot->readChannelType != nullptr) && (_iot->numberReadChannel > 0))
    {
        _iot->readChannelType = static_cast<uint8_t *>(malloc(_iot->numberReadChannel));
        if (_iot->readChannelType != nullptr)
            std::memcpy(_iot->readChannelType, iot->readChannelType, _iot->numberReadChannel);
    }

    // writeChannelType
    if ((_iot->writeChannelType != nullptr) && (_iot->numberWriteChannel > 0))
    {
        _iot->writeChannelType = static_cast<uint8_t *>(malloc(_iot->numberWriteChannel));
        if (_iot->writeChannelType != nullptr)
            std::memcpy(_iot->writeChannelType, iot->writeChannelType, _iot->numberWriteChannel);
    }

    // name
    if ((_iot->name != nullptr) && (_iot->nameSize > 0))
    {
        _iot->name = static_cast<char *>(malloc(_iot->nameSize));
        if (_iot->name != nullptr)
            std::memcpy(_iot->name, iot->name, _iot->nameSize);
    }

    // description
    if ((_iot->description != nullptr) && (_iot->descriptionSize > 0))
    {
        _iot->description = static_cast<char *>(malloc(_iot->descriptionSize));
        if (_iot->description != nullptr)
            std::memcpy(_iot->description, iot->description, _iot->descriptionSize);
    }
}

RAII_iot::~RAII_iot()
{
    clear_iotv_obj(_iot);
}

const iotv_obj_t *RAII_iot::iot() const
{
    return _iot;
}
