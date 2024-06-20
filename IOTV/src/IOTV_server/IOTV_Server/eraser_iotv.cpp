#include "eraser_iotv.h"
#include "qthread.h"

void Eraser_iotv::client(std::unordered_map<IOTV_Client *, QThread *> &_iot_client, IOTV_Client *client)
{
    if (client == nullptr)
        return;

    client->deleteLater();

    _iot_client[client]->exit();
    _iot_client[client]->wait();
    delete _iot_client[client];

    _iot_client.erase(client);
}

void Eraser_iotv::clients(std::unordered_map<IOTV_Client *, QThread *> &_iot_client)
{
    for(auto &[client, thread] : _iot_client)
    {
        if (client != nullptr)
            client->deleteLater();

        if (thread != nullptr)
        {
            thread->exit();
            thread->wait();
            delete thread;
        }
    }
    _iot_client.clear();
}

void Eraser_iotv::host(std::unordered_map<IOTV_Host *, QThread *> &_iot_host, IOTV_Host *host)
{
    if (host == nullptr)
        return;

    host->deleteLater();

    _iot_host[host]->exit();
    _iot_host[host]->wait();
    delete _iot_host[host];

    _iot_host.erase(host);
}

void Eraser_iotv::hosts(std::unordered_map<IOTV_Host *, QThread *> &_iot_host)
{
    for(auto &[host, thread] : _iot_host)
    {
        if (host != nullptr)
            host->deleteLater();

        if (thread != nullptr)
        {
            thread->exit();
            thread->wait();
            delete thread;
        }
    }
    _iot_host.clear();
}

void Eraser_iotv::bot(IOTV_Bot **_tg_bot, QThread **_tgbot_thread)
{
    if (*_tgbot_thread != nullptr)
    {
        (*_tgbot_thread)->exit();
        (*_tgbot_thread)->wait();
        delete *_tgbot_thread;
        *_tgbot_thread = nullptr;
    }

    if (*_tg_bot != nullptr)
    {
        delete *_tg_bot;
        *_tg_bot = nullptr;
    }
}
