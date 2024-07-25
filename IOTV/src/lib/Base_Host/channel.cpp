#include "channel.h"
#include "log.h"

void Channel::addSubchannel(const Raw &data)
{
    _data.push_back(data);
}

bool Channel::removeSubchannel(uint8_t channelNumber)
{
    try
    {
        if (channelNumber > _data.size())
            throw std::out_of_range{"out of range"};

        _data.erase(_data.begin() + channelNumber);
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
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
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
    catch (std::invalid_argument &ex)
    {
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
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
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
    catch (std::invalid_argument &ex)
    {
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
}

bool Channel::addData(uint8_t channelNumber, const QByteArray &data)
{
    auto type = getType(channelNumber);

    if (type != Raw::DATA_TYPE::RAW && type != Raw::DATA_TYPE::STRING)
    {
        Log::write(CATEGORY::WARNING, QString("add data to type != Raw::DATA_TYPE::RAW and type != Raw::DATA_TYPE::RAW ") + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }

    try
    {
        _data.at(channelNumber).addData(data);
        return true;
    }
    catch (std::out_of_range &ex)
    {
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
    catch (std::invalid_argument &ex)
    {
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return false;
    }
}

void Channel::clearData(uint8_t channelNumber)
{
    try
    {
        _data.at(channelNumber).clear();
    }
    catch (std::out_of_range &ex)
    {
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
    }
}

const QByteArray &Channel::getData(uint8_t channelNumber) const
{
    try
    {
        return _data.at(channelNumber).data();
    }
    catch (std::out_of_range &ex)
    {
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);

        static const QByteArray cap;
        return cap;
    }
}

const Raw &Channel::getRawData(uint8_t channelNumber) const
{
    try
    {
        return _data.at(channelNumber);
    }
    catch (std::out_of_range &ex)
    {
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);

        static const Raw cap;
        return cap;
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
        Log::write(CATEGORY::WARNING, QString(ex.what()) + " " + QString(Q_FUNC_INFO),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return Raw::DATA_TYPE::NONE;
    }
}

uint8_t Channel::size() const
{
    return _data.size();
}
