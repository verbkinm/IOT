#pragma once_H

#include "IOTV_Host/iotv_host.h"
#include "IOTV_Client/iotv_client.h"
#include "IOTV_Bot/iotv_bot.h"

class Eraser_iotv
{
public:
    Eraser_iotv() = delete;

    static void client(std::unordered_map<IOTV_Client *, QThread *> &_iot_client, IOTV_Client *client);
    static void clients(std::unordered_map<IOTV_Client *, QThread *> &_iot_client);

    static void host(std::unordered_map<IOTV_Host *, QThread *> &_iot_host, IOTV_Host *host);
    static void hosts(std::unordered_map<IOTV_Host *, QThread *> &_iot_host);

    static void bot(IOTV_Bot **_tg_bot, QThread **_tgbot_thread);
};

