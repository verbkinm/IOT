#include "iotv_action_msg.h"

IOTV_Action_MSG::IOTV_Action_MSG(const QString &msg, QObject *parent) :
    IOTV_Action(ACTION_TYPE::MESSAGE, parent),
    _msg(msg)
{

}

void IOTV_Action_MSG::forceExec()
{
    if (!isValid())
        return;
}

void IOTV_Action_MSG::exec()
{
    if (!isEnable()  || !isValid())
        return;

    Log::write(CATEGORY::ACTION, _msg, Log::Write_Flag::STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
}

bool IOTV_Action_MSG::isValid() const
{
    return true;
}
