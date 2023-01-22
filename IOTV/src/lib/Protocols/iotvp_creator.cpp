#include "iotvp_creator.h"

IOTVP_Creator::IOTVP_Creator(const QByteArray &data) : _error(false),
    _expectedDataSize(0), _cutDataSize(0), _headerCheckSumMustBe(0), _headerDataSizeMustBe(0),
    _bodyCheckSumMustBe(0), _bodyDataSizeMustBe(0),
    _rawData(data)
{
}

void IOTVP_Creator::createPkgs()
{
    if (static_cast<uint64_t>(_rawData.size()) < IOTVP_Abstract::HEADER_SIZE) // Заголовок протокола (20 байт + N байт, N максимум 2^48) (документация)
    {
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE;
        _error = false;
        return;
    }

    createHeader();

    // Ошибка или данные тела протокола ещё не все пришли,
    if (_error || (_expectedDataSize > 0))
    {
        _cutDataSize = 0;
        return;
    }

    // Тело протокола для данного заголовка не предусмотрено
    if (!bodyMustBe(_header->assignment()))
    {
        _error = false;         // ошибок нет
        _expectedDataSize = 0;  // пакет пришел полностью
        _cutDataSize = IOTVP_Abstract::HEADER_SIZE; // пакет размером HEADER_SIZE байт
        return;
    }

    createBody();

    if ((_body == nullptr) && (_expectedDataSize == 0))
    {
        _error = true;
        _expectedDataSize = 0;
        _cutDataSize = 0;
        return;
    }

    _header->setBody(std::move(_body));
    if (!_error && (_expectedDataSize == 0))
        _cutDataSize = _header->size();

    if (_expectedDataSize > 0)
        return;

    if ( (_header->dataSize() != _headerDataSizeMustBe) || (_header->checkSum() != _headerCheckSumMustBe)
        || ( (_body != nullptr) && ((_body->dataSize() != _bodyDataSizeMustBe) || (_body->checkSum() != _bodyCheckSumMustBe)) ) )
    {
        _error = true;
        _expectedDataSize = 0;
        _cutDataSize = 0;
    }
}

void IOTVP_Creator::createHeader()
{
    // Данные не полные.
    if (static_cast<uint64_t>(_rawData.size()) < IOTVP_Abstract::HEADER_SIZE) // Заголовок протокола (20 байт + N байт, N максимум 2^48) (документация)
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE;
        return;
    }

    const uint8_t version = _rawData[0];
    const uint8_t type = _rawData[1];
    const uint8_t assigment = _rawData[2];
    const uint8_t flags = _rawData[3];

    QByteArray buf = _rawData.sliced(4, 8);
    uint64_t dataSize;
    std::memcpy(&dataSize, &buf[0], 8);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        dataSize = qToBigEndian(dataSize);
    _headerDataSizeMustBe = dataSize;


    buf = _rawData.sliced(12, 8);
    uint64_t chSum;
    std::memcpy(&chSum, &buf[0], 8);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);
    _headerCheckSumMustBe = chSum;

    uint64_t sum = version + type + assigment + flags + dataSize;
    if (sum != chSum)
    {
        _error = true;
        _expectedDataSize = 0;
        return;
    }

    // Данные тела протокола ещё не все пришли
    if (static_cast<uint64_t>(_rawData.size()) < (IOTVP_Abstract::HEADER_SIZE + dataSize))
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + dataSize;
        return;
    }

    _header = std::make_unique<IOTVP_Header>();
    _header->setVersion(version);
    _header->setType(static_cast<IOTVP_Header::TYPE>(type));
    _header->setAssignment(static_cast<IOTVP_Header::ASSIGNMENT>(assigment));
    _header->setFlags(static_cast<IOTVP_Header::FLAGS>(flags));
}

void IOTVP_Creator::createBody()
{
    switch (_header->assignment())
    {
        case IOTVP_Header::ASSIGNMENT::NONE :
        case IOTVP_Header::ASSIGNMENT::PING_PONG :
        {
            Q_ASSERT(true);
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

    return false;
}

std::unique_ptr<IOTVP_Header> IOTVP_Creator::takeHeader()
{
    return std::move(_header);
}

std::unique_ptr<IOTVP_AbstractBody> IOTVP_Creator::takeBody()
{
    return std::move(_body);
}

uint64_t IOTVP_Creator::expectedDataSize() const
{
    return _expectedDataSize;
}

uint64_t IOTVP_Creator::cutDataSize() const
{
    return _cutDataSize;
}

bool IOTVP_Creator::error() const
{
    return _error;
}

void IOTVP_Creator::createBodyIdentification()
{
    // Данные не полные.
    if (static_cast<uint64_t>(_rawData.size()) < IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::IDENTIFICATION_SIZE)
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::IDENTIFICATION_SIZE;
        return;
    }

    const uint16_t id = (_rawData[IOTVP_Abstract::HEADER_SIZE + 0] << 8) | (_rawData[IOTVP_Abstract::HEADER_SIZE + 1]);
    const uint8_t nameSize = _rawData[IOTVP_Abstract::HEADER_SIZE + 2];
    const uint16_t desctiptionSize = (_rawData[IOTVP_Abstract::HEADER_SIZE + 3] << 8) | (_rawData[IOTVP_Abstract::HEADER_SIZE + 4]);
    const uint8_t numberWriteChannel= _rawData[IOTVP_Abstract::HEADER_SIZE + 5];
    const uint8_t numberReadChannel  = _rawData[IOTVP_Abstract::HEADER_SIZE + 6];
    const uint8_t flags = _rawData[IOTVP_Abstract::HEADER_SIZE + 7];

    _bodyDataSizeMustBe = 0;

    QByteArray buf = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + 8, 8);
    uint64_t chSum;

    std::memcpy(&chSum, &buf[0], 8);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);
    _bodyCheckSumMustBe = chSum;

    uint64_t sum = id + nameSize + desctiptionSize + numberWriteChannel + numberReadChannel + flags;
    if (sum != chSum)
    {
        _error = true;
        _expectedDataSize = 0;
        return;
    }
    if (static_cast<uint64_t>(_rawData.size()) < (IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::IDENTIFICATION_SIZE + nameSize + desctiptionSize + numberWriteChannel + numberReadChannel))
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::IDENTIFICATION_SIZE + nameSize + desctiptionSize + numberWriteChannel + numberReadChannel;
        return;
    }

    QString name;
    if (nameSize > 0)
        name = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::IDENTIFICATION_SIZE, nameSize);

    QString description;
    if (desctiptionSize > 0)
        description = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::IDENTIFICATION_SIZE + nameSize, desctiptionSize);

    QList<Raw::DATA_TYPE> writeChannel;
    for (uint64_t i = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::IDENTIFICATION_SIZE + nameSize + desctiptionSize, j = 0; j < numberWriteChannel; i++, j++)
        writeChannel.push_back(static_cast<Raw::DATA_TYPE>(_rawData.at(i)));

    QList<Raw::DATA_TYPE> readChannel;
    for (uint64_t i = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::IDENTIFICATION_SIZE + nameSize + desctiptionSize + numberWriteChannel, j = 0; j < numberReadChannel; i++, j++)
        readChannel.push_back(static_cast<Raw::DATA_TYPE>(_rawData.at(i)));

    auto body = std::make_unique<IOTVP_Identification>();
    body->setId(id);
    body->setName(name);
    body->setDescription(description);
    body->setWriteChannel(std::move(writeChannel));
    body->setReadChannel(std::move(readChannel));
    body->setFlags(static_cast<IOTVP_Identification::FLAGS>(flags));

    _body = std::move(body);
}

void IOTVP_Creator::createBodyState()
{
    // Данные не полные.
    if (static_cast<uint64_t>(_rawData.size()) < IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::STATE_SIZE)
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::STATE_SIZE;
        return;
    }

    const uint8_t nameSize = _rawData[IOTVP_Abstract::HEADER_SIZE + 0];
    const uint8_t state = _rawData[IOTVP_Abstract::HEADER_SIZE + 1];
    const uint8_t flags = _rawData[IOTVP_Abstract::HEADER_SIZE + 2];

    QByteArray buf = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + 3, 4);
    uint32_t dataSize;

    std::memcpy(&dataSize, &buf[0], 4);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        dataSize = qToBigEndian(dataSize);
    _bodyDataSizeMustBe = dataSize;

    buf = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + 7, 8);
    uint64_t chSum;

    std::memcpy(&chSum, &buf[0], 8);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);
    _bodyCheckSumMustBe = chSum;

    uint64_t sum = nameSize + state + flags + dataSize;
    if (sum != chSum)
    {
        _error = true;
        _expectedDataSize = 0;
        return;
    }
    if (static_cast<uint64_t>(_rawData.size()) < (IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::STATE_SIZE + nameSize + dataSize))
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::STATE_SIZE + nameSize + dataSize;
        return;
    }

    QString name;
    if (nameSize > 0)
        name = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::STATE_SIZE, nameSize);

    QByteArray data;
    if (dataSize > 0)
        data = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::STATE_SIZE + nameSize, dataSize);

    //!!!
    // Данные тела протокола ещё не все пришли
    if (static_cast<uint64_t>(_rawData.size()) < (IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::STATE_SIZE + dataSize))
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::STATE_SIZE + dataSize;
        return;
    }

    auto body = std::make_unique<IOTVP_State>();
    body->setName(name);
    body->setState(static_cast<IOTVP_State::STATE>(state));
    body->setFlags(static_cast<IOTVP_State::FLAGS>(flags));
    body->setData(data);

    _body = std::move(body);
}

void IOTVP_Creator::createBodyReadWrite()
{
    // Данные не полные.
    if (static_cast<uint64_t>(_rawData.size()) < IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::READWRITE_SIZE)
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::READWRITE_SIZE;
        return;
    }

    const uint8_t nameSize = _rawData[IOTVP_Abstract::HEADER_SIZE + 0];
    const uint8_t channelNumber = _rawData[IOTVP_Abstract::HEADER_SIZE + 1];
    const uint8_t flags = _rawData[IOTVP_Abstract::HEADER_SIZE + 2];

    QByteArray buf = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + 3, 4);
    uint32_t dataSize;

    std::memcpy(&dataSize, &buf[0], 4);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        dataSize = qToBigEndian(dataSize);
    _bodyDataSizeMustBe = dataSize;

    buf = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + 7, 8);
    uint64_t chSum;

    std::memcpy(&chSum, &buf[0], 8);
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);
    _bodyCheckSumMustBe = chSum;

    uint64_t sum = nameSize + channelNumber + flags + dataSize;
    if (sum != chSum)
    {
        _error = true;
        _expectedDataSize = 0;
        return;
    }

    if (static_cast<uint64_t>(_rawData.size()) < (IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::READWRITE_SIZE + nameSize + dataSize))
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::READWRITE_SIZE + nameSize + dataSize;
        return;
    }

    QString name;
    if (nameSize > 0)
        name = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::READWRITE_SIZE, nameSize);

    QByteArray data;
    if (dataSize > 0)
        data = _rawData.sliced(IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::READWRITE_SIZE + nameSize, dataSize);

    //!!!
    // Данные тела протокола ещё не все пришли
    if (static_cast<uint64_t>(_rawData.size()) < (IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::READWRITE_SIZE + dataSize))
    {
        _error = false;
        _expectedDataSize = IOTVP_Abstract::HEADER_SIZE + IOTVP_Abstract::READWRITE_SIZE + dataSize;
        return;
    }

    auto body = std::make_unique<IOTVP_READ_WRITE>();
    body->setName(name);
    body->setChannelNumber(channelNumber);
    body->setFlags(static_cast<IOTVP_State::FLAGS>(flags));
    body->setData(data);

    _body = std::move(body);
}
