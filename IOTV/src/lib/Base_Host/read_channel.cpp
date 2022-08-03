#include "read_channel.h"

bool Read_Channel::addSubchannel(const Raw::DATA_TYPE dataType)
{
    _dataType.push_back(dataType);
    _data.emplace_back(Raw{});

    //!!!
    return true;
}

Read_Channel::~Read_Channel()
{
    for(size_t i = 0; i < _data.size(); i++)
        clearPointerRAW(i);
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
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Write_Flag::FILE_STDERR);
        return false;
    }

}

void Read_Channel::removeAllSubchanel()
{
    for(size_t i = 0; i < _data.size(); i++)
        clearPointerRAW(i);

    _data.clear();
    _dataType.clear();
}

bool Read_Channel::setData(uint8_t index, Raw rawData)
{
    try
    {
        _data.at(index) = rawData;
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Write_Flag::FILE_STDERR);
        return false;
    }
}

Raw Read_Channel::getData(uint8_t index) const
{
    try
    {
        return _data.at(index);
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Write_Flag::FILE_STDERR);
        return {};
    }
}

bool Read_Channel::clearPointerRAW(uint8_t index)
{
    try
    {
        Raw::RAW &raw = _data.at(index);
        if(_dataType.at(index) == Raw::DATA_TYPE::CHAR_PTR && raw.str != nullptr)
        {
            delete[] raw.str;
            raw.str = nullptr;
            return true;
        }

        return false;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Write_Flag::FILE_STDERR);
        return false;
    }
}
