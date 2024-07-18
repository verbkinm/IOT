#include "tg_bot_handler.h"
#include "ConfigTypes.h"

#include <QFile>

namespace tg_bot_handler {

void cmd_clients(IOTV_Bot *tg_bot, int64_t id)
{
    if (tg_bot == nullptr)
        return;

    QFile file(ServerLog::CLIENT_ONLINE_LOG_FILENAME);
    file.open(QIODevice::ReadOnly);

    QString txt = "<b>Список клиентов:</b>\n\n";
    QString dataFile = file.readAll();

    if (dataFile.size() == 0)
        txt += "Нет подключенных клиентов";
    else
        txt += dataFile;

    emit tg_bot->signalSendMsg(id, txt);
}

void cmd_hosts(IOTV_Bot *tg_bot, int64_t id, const std::unordered_map<IOTV_Host *, QThread *> &iot_hosts)
{
    if (tg_bot == nullptr)
        return;

    QString txt = "<b>Список хостов:</b>\n\n";

    int i = 1;
    for (const auto &[host, _] : iot_hosts)
    {

        txt += QString::number(i) + ": " + host->getName() + " (" + host->getAddress() + ") - " + (host->state() == State_STATE_ONLINE ? "онлайн" : "офлайн") + "\n";
        ++i;
    }

    if (iot_hosts.size() == 0)
        txt += "Нет подключенных хостов";

    emit tg_bot->signalSendMsg(id, txt);
}

void cmd_conf_server(IOTV_Bot *tg_bot, int64_t id, const QString &fileName)
{
    if (tg_bot == nullptr)
        return;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QString txt = "<b>Конфигурационный файл сервера:</b>\n\n" + file.readAll();

    emit tg_bot->signalSendMsg(id, txt);
}

void cmd_conf_hosts(IOTV_Bot *tg_bot, int64_t id, const QString &fileName)
{
    if (tg_bot == nullptr)
        return;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QString txt = "<b>Конфигурационный файл хостов:</b>\n\n" + file.readAll();

    emit tg_bot->signalSendMsg(id, txt);
}

}
