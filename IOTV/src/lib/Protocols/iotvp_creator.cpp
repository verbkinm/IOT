#include "iotvp_creator.h"

IOTVP_Creator::IOTVP_Creator(QByteArray &data) : _error(false), _complete(false),
    _rawData(data), _expectedDataSize(0)
{

}

std::pair<std::unique_ptr<IOTVP_Header>,
std::unique_ptr<IOTVP_AbstractBody> >
IOTVP_Creator::createPkg()
{
    if (static_cast<uint64_t>(_rawData.size()) < IOTVP_Abstract::HEADER_SIZE) // Заголовок протокола (20 байт + N байт, N максимум 2^48) (документация)
    {
        _error = false;
        _complete = false;
        return {};
    }

    createHeader();
    if (_error)
        return {};

    if (!bodyMustBe(_header->assignment()))
    {
        // Обрезаем по длине хедера
        _rawData = _rawData.mid(_header->size());
        return {std::move(_header), nullptr};
    }

    createBody();
    if (_error)
        return {};

    if (!_complete)
        return{};

    return {std::move(_header), std::move(_body)};
}

void IOTVP_Creator::createHeader() const
{
    // Данные не полные.
    if (static_cast<uint64_t>(_rawData.size()) < IOTVP_Abstract::HEADER_SIZE) // Заголовок протокола (20 байт + N байт, N максимум 2^48) (документация)
    {
        _complete = false;
        _error = false;
        return;
    }

    const uint8_t version = _rawData[0];
    const uint8_t type = _rawData[1];
    const uint8_t assigment = _rawData[2];
    const uint8_t flags = _rawData[3];

    uint64_t dataSize;
    std::memcpy(&dataSize, &_rawData[4], 8);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        dataSize = qToBigEndian(dataSize);

    uint64_t chSum;
    std::memcpy(&chSum, &_rawData[12], 8);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);

    uint64_t sum = version + type + assigment + flags + dataSize;
    if (sum != chSum)
    {
        _complete = false;
        _error = true;
        _rawData.clear();
        return;
    }

    // Данные тела протокола ещё не все пришли
    if (static_cast<uint64_t>(_rawData.size()) < (IOTVP_Abstract::HEADER_SIZE + dataSize))
    {
        _complete = false;
        _error = false;
        return;
    }

    _header = std::make_unique<IOTVP_Header>();
    _header->setVersion(version);
    _header->setType(static_cast<IOTVP_Header::TYPE>(type));
    _header->setAssignment(static_cast<IOTVP_Header::ASSIGNMENT>(assigment));
    _header->setFlags(static_cast<IOTVP_Header::FLAGS>(flags));
}

void IOTVP_Creator::createBody() const
{
    switch (_header->assignment())
    {
        case IOTVP_Header::ASSIGNMENT::NONE :
        {
            break;
        }
        case IOTVP_Header::ASSIGNMENT::IDENTIFICATION :
        {
            createBodyIdentification();
            break;
        }
        case IOTVP_Header::ASSIGNMENT::STATE :
        {
            createBodyState();
            break;
        }
        case IOTVP_Header::ASSIGNMENT::READ :
        case IOTVP_Header::ASSIGNMENT::WRITE :
        {
            createBodyReadWrite();
            break;
        }
        case IOTVP_Header::ASSIGNMENT::PING_PONG :
        {

            break;
        }
    }

    if (!_complete)
    {
        _error = false;
    }
}

bool IOTVP_Creator::bodyMustBe(IOTVP_Header::ASSIGNMENT assigment) const
{
    switch (assigment)
    {
        case IOTVP_Header::ASSIGNMENT::IDENTIFICATION :
        case IOTVP_Header::ASSIGNMENT::STATE :
        case IOTVP_Header::ASSIGNMENT::READ :
        case IOTVP_Header::ASSIGNMENT::WRITE :
            return true;
        case IOTVP_Header::ASSIGNMENT::NONE :
        case IOTVP_Header::ASSIGNMENT::PING_PONG :
            return false;
    }
}

uint64_t IOTVP_Creator::expectedDataSize() const
{
    return _expectedDataSize;
}

bool IOTVP_Creator::error() const
{
    return _error;
}

void IOTVP_Creator::createBodyIdentification() const
{

}

void IOTVP_Creator::createBodyState() const
{

}

void IOTVP_Creator::createBodyReadWrite() const
{

}
