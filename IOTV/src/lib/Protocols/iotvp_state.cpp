#include "iotvp_state.h"

IOTVP_STATE::IOTVP_STATE() :
    _state(0)
{

}

uint8_t IOTVP_STATE::state() const
{
    return _state;
}

uint16_t IOTVP_STATE::checkSum() const
{
    return nameSize() + state() + flags() + dataSize();
}

uint64_t IOTVP_STATE::size() const
{
    // Состояние устройства (23 + N байт, N максимум 2^40) (документация)
    return 23 + nameSize() + dataSize();
}

void IOTVP_STATE::setState(uint8_t newState)
{
    _state = newState;
}

QByteArray IOTVP_STATE::toData() const
{
    QByteArray result(size(), 0);
    result[0] = nameSize();
    result[1] = state();
    result[2] = flags();

    auto dSize = dataSize();
    std::memcpy(&result[3], &dSize, 4); // Размер пакета данных = 4 байта (документация)

    auto chSum = checkSum();
    std::memcpy(&result[7], &chSum, 16); // Контрольная сумма тела пакетах = 16 байта (документация)

    std::memcpy(&result[7 + 16], name().toStdString().c_str(), nameSize());
    std::memcpy(&result[7 + 16 + nameSize()], data().toStdString().c_str(), dataSize());

    return result;
}
