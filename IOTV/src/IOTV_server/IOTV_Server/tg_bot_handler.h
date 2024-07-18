#pragma once

#include "IOTV_Bot/iotv_bot.h"
#include "IOTV_Host/iotv_host.h"

#include <QThread>

namespace tg_bot_handler {

void cmd_clients(IOTV_Bot *tg_bot, int64_t id);
void cmd_hosts(IOTV_Bot *tg_bot, int64_t id, const std::unordered_map<IOTV_Host *, QThread *>  &iot_hosts);
void cmd_conf_server(IOTV_Bot *tg_bot, int64_t id, const QString &fileName);
void cmd_conf_hosts(IOTV_Bot *tg_bot, int64_t id, const QString &fileName);

}
