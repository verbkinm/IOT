#include "read_channel.h"

bool Read_Channel::addSubchannel(const Raw::DATA_TYPE dataType)
{
    _dataType.push_back(dataType);
    Raw::RAW rawData;
    rawData.ui64 = 0;
    _data.push_back(rawData);

    return true;
}

Read_Channel::~Read_Channel()
{
    for(size_t i = 0; i < _data.size(); i++)
    {
        if(_dataType.at(i) == Raw::DATA_TYPE::CHAR_PTR && _data.at(i).str != nullptr)
            delete[] _data.at(i).str;

        _dataType.erase(_dataType.begin() + i);
        _data.erase(_data.begin() + i);
    }
}

bool Read_Channel::removeSubchannel(uint8_t index)
{
    try
    {
        if(index >= _dataType.size())
            throw std::out_of_range{"out of range"};

        if(_dataType.at(index) == Raw::DATA_TYPE::CHAR_PTR && _data.at(index).str != nullptr)
            delete[] _data.at(index).str;

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
    for(size_t i = 0; i < _data.size(); i++)
        removeSubchannel(i);

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

bool Read_Channel::clearPointerRAW(uint8_t index)
{
    try
    {
        Raw::RAW &raw = _data.at(index);
        if(raw.str != nullptr)
        {
            delete[] raw.str;
            raw.str = nullptr;
            return true;
        }

        return false;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Flags::WRITE_TO_FILE_AND_STDERR);
        return false;
    }
}
