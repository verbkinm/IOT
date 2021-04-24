#include "read_channel.h"

bool Read_Channel::addSubchannel(const Raw::DATA_TYPE dataType)
{
    _dataType.push_back(dataType);
    Raw::RAW rawData;
    rawData.ui64 = 0;
    _data.push_back(rawData);

    return true;
}

bool Read_Channel::removeSubchannel(uint8_t index)
{
    try
    {
        if(index >= _dataType.size())
            throw std::out_of_range{"out of range"};

        _dataType.erase(_dataType.begin() + index);
        _data.erase(_data.begin() + index);
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Flags::WRITE_TO_FILE_AND_STDERR);
        return false;
    }

}

void Read_Channel::removeAllSubchanel()
{
    _data.clear();
    _dataType.clear();
}

bool Read_Channel::setData(uint8_t index, Raw::RAW rawData)
{
    try
    {
        _data.at(index) = rawData;
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Flags::WRITE_TO_FILE_AND_STDERR);
        return false;
    }
}

Raw::RAW Read_Channel::getData(uint8_t index) const
{
    try
    {
        return _data.at(index);
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Flags::WRITE_TO_FILE_AND_STDERR);
        Raw::RAW type;
        type.b = false;
        return type;
    }
}
