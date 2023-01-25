#include "iotvp_state.h"

IOTVP_State::IOTVP_State() : IOTVP_AbstractReadWrite(BODY_TYPE::STATE), _state(STATE::OFFLINE)
{
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
    return IOTVP_Abstract::STATE_SIZE + nameSize() + dataSize();
}

void IOTVP_State::setState(STATE newState)
{
    _state = newState;
}

bool IOTVP_State::operator==(const IOTVP_Abstract &obj) const
{
    const IOTVP_State *ptr = dynamic_cast<const IOTVP_State*>(&obj);
    if (ptr == nullptr)
        return false;

    return  ( (state() == ptr->state())
              && (flags() == ptr->flags())
              && (name() == ptr->name())
              && (data() == ptr->data()) );
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
    std::memcpy(&result[3], &dSize, 4); // Размер пакета данных = 4 байта (документация)

    auto chSum = checkSum();
    if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
        chSum = qToBigEndian(chSum);
    std::memcpy(&result[7], &chSum, 8); // Контрольная сумма тела пакетах = 8 байта (документация)

    if (nameSize() > 0)
        std::memcpy(&result[7 + 8], name().toStdString().c_str(), nameSize());

    if (dataSize() > 0)
    {
//        auto rawData = data();
//        if (Q_BYTE_ORDER == Q_LITTLE_ENDIAN)
//            rawData = qToBigEndian(rawData);
        std::memcpy(&result[7 + 8 + nameSize()], data().toStdString().c_str(), dataSize());
    }

    return result;
}
