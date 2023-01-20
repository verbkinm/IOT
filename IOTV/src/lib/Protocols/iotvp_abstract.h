#pragma once

#include <QByteArray>
#include <memory>

class IOTVP_Abstract
{
public:
    enum class BODY_TYPE : uint8_t
    {
        NONE = 0,
        IDENTIFICATION,
        STATE,
        READ_WRITE
    };

    enum class FLAGS : uint8_t
    {
        NONE = 0,
        ERROR = 0xFF
    };

    const static uint64_t HEADER_SIZE = 20;         // Заголовок протокола (20 байт + N байт, N максимум 248) (документация)
    const static uint64_t IDENTIFICATION_SIZE = 16; // Идентификация устройства (16 + N байт, N максимум 2^40) (документация)
    const static uint64_t READWRITE_SIZE = 15;      // Чтение / Запись данных (15 + N байт, N максимум 2^40) (документация)
    const static uint64_t STATE_SIZE = 15;          // Состояние устройства (15 + N байт, N максимум 2^40) (документация)

    virtual ~IOTVP_Abstract() = default;

    virtual uint64_t checkSum() const = 0;

    virtual uint64_t dataSize() const = 0;;
    virtual uint64_t size() const = 0;

    virtual QByteArray toData() const = 0;

    virtual bool operator==(const IOTVP_Abstract &) const = 0;
};

