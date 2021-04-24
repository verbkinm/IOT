#include "channel.h"

bool Channel::setDataType(uint8_t index, Raw::DATA_TYPE dataType)
{
    try
    {
        _dataType.at(index) = dataType;
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Flags::WRITE_TO_FILE_AND_STDERR);
        return false;
    }
    catch (std::invalid_argument &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Flags::WRITE_TO_FILE_AND_STDERR);
        return false;
    }
}

Raw::DATA_TYPE Channel::getDataType(uint8_t index) const
{
    try
    {
        return _dataType.at(index);
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO), Log::Flags::WRITE_TO_FILE_AND_STDERR);
        return Raw::DATA_TYPE::RAW;
    }
}

uint8_t Channel::length() const
{
    return _dataType.size();
}
