#pragma once

#include "base_host.h"

class IOTV_Event : public QObject
{
    Q_OBJECT
public:
    enum class EVENT_TYPE : uint8_t
    {
        NONE = 0,
        CONNECTING,
        DISCONNECTING,
        STATE,
        DATA,
    };

    enum class COMPARE : uint8_t
    {
        NONE,           // нет сравнения
        EQUAL,          // ==
        NOT_EQUAL,      // !=
        LESS,           // <
        GREATER,        // >
        LESS_OR_EQUAL,  // <=
        GREATER_OR_EQUAL// >=
    };

    IOTV_Event(EVENT_TYPE type, const Base_Host *host,
               QObject *parent = nullptr);

    void setCompare(COMPARE newCompare);
    COMPARE compare() const;

    const Base_Host *host() const;

    EVENT_TYPE type() const;

    bool isValid() const;

    friend bool operator==(const IOTV_Event &lhs, const IOTV_Event &rhs);
    friend bool operator<(const IOTV_Event &lhs, const IOTV_Event &rhs);

protected:
    const Base_Host *_host;

private:
    EVENT_TYPE _type;
    COMPARE _compare;

signals:
    void signalEvent();
};

template <typename T1, typename T2>
bool compare(T1 arg1, T2 arg2, IOTV_Event::COMPARE opearation)
{
    switch (opearation)
    {
    case IOTV_Event::COMPARE::NONE:
        return true;
        break;
    case IOTV_Event::COMPARE::EQUAL:
        return arg1 == arg2;
        break;
    case IOTV_Event::COMPARE::NOT_EQUAL:
        return arg1 != arg2;
        break;
    case IOTV_Event::COMPARE::LESS:
        return arg1 < arg2;
        break;
    case IOTV_Event::COMPARE::GREATER:
        return arg1 > arg2;
        break;
    case IOTV_Event::COMPARE::LESS_OR_EQUAL:
        return arg1 <= arg2;
        break;
    case IOTV_Event::COMPARE::GREATER_OR_EQUAL:
        return arg1 >= arg2;
        break;
    default:
        return false;
        break;
    }

    return false;
}

