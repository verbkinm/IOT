#include "channel.h"

void Channel::addSubchannel(const Raw &data)
{
    _data.push_back(data);
}

bool Channel::removeSubchannel(uint8_t channelNumber)
{
    try
    {
        if(channelNumber > _data.size())
            throw std::out_of_range{"out of range"};

        _data.erase(_data.begin() + channelNumber);
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
}

void Channel::removeAllSubchanel()
{
    _data.clear();
}

bool Channel::setData(uint8_t channelNumber, const Raw &data)
{
    try
    {
        _data.at(channelNumber) = data;
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
    catch (std::invalid_argument &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
}

bool Channel::setData(uint8_t channelNumber, const QByteArray &data)
{
    try
    {
        _data.at(channelNumber).setData(data);
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
    catch (std::invalid_argument &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
}

QByteArray Channel::getData(uint8_t channelNumber) const
{
    try
    {
        return _data.at(channelNumber).data();
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return {};
    }
}

Raw Channel::getRawData(uint8_t channelNumber) const
{
    try
    {
        return _data.at(channelNumber);
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return {};
    }
}

Raw::DATA_TYPE Channel::getType(uint8_t channelNumber) const
{
    try
    {
        return _data.at(channelNumber).type();
    }
    catch (std::out_of_range &ex)
    {
        Log::write(QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return Raw::DATA_TYPE::NONE;
    }
}

uint8_t Channel::size() const
{
    return _data.size();
}
