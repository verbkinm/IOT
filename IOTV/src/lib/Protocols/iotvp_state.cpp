#include "iotvp_state.h"

IOTVP_State::IOTVP_State() : _state(STATE::OFFLINE)
{
    _bodyType = BODY_TYPE::STATE;
}

IOTVP_State::STATE IOTVP_State::state() const
{
    return _state;
}

uint64_t IOTVP_State::checkSum() const
{
    return nameSize() + static_cast<uint8_t>(state()) + static_cast<uint8_t>(flags()) + dataSize();
}

uint64_t IOTVP_State::size() const
{
    // Состояние устройства (15 + N байт, N максимум 2^40) (документация)
    return 15 + nameSize() + dataSize();
}

void IOTVP_State::setState(STATE newState)
{
    _state = newState;
}

QByteArray IOTVP_State::toData() const
{
    QByteArray result(size(), 0);
    result[0] = nameSize();
    result[1] = static_cast<uint8_t>(state());
    result[2] = static_cast<uint8_t>(flags());

    auto dSize = dataSize();
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        dSize = qToBigEndian(dSize);
    std::memcpy((void *)&result[3], &dSize, 4); // Размер пакета данных = 4 байта (документация)

    auto chSum = checkSum();
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);
    std::memcpy(&result[7], &chSum, 8); // Контрольная сумма тела пакетах = 8 байта (документация)

    if (nameSize() > 0)
        std::memcpy(&result[7 + 8], name().toStdString().c_str(), nameSize());

    if (dataSize() > 0)
        std::memcpy(&result[7 + 8 + nameSize()], data().toStdString().c_str(), dataSize());

    return result;
}
