#include "write_channel.h"

bool Write_Channel::addSubchannel(Raw::DATA_TYPE dataType)
{
    _dataType.push_back(dataType);

    return true;
}

bool Write_Channel::removeSubchannel(uint8_t index)
{
    try
    {
        if(index > _dataType.size())
            throw std::out_of_range{"out of range"};

        _dataType.erase(_dataType.begin() + index);
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Write_Flag::FILE_STDERR);
        return false;
    }
}

void Write_Channel::removeAllSubchanel()
{
    _dataType.clear();
}
