#pragma once

#include <QUdpSocket>

#include "IOTV_Host/iotv_host.h"
#include "IOTV_Client/iotv_client.h"
#include "IOTV_Bot/iotv_bot.h"

class Maker_iotv
{
public:
    Maker_iotv() = delete;

    static IOTV_Host *host_tcp_in(std::unordered_map<IOTV_Host * , QThread *> &add_to_iot_hosts, uint maxHostCount,
                           QTcpSocket *reverse_socket, QObject *parent);

    static IOTV_Host *host(std::unordered_map<IOTV_Host* , QThread*> &add_to_iot_hosts, uint maxHostCount,
                                    const std::unordered_map<QString, QString> &setting,
                                    QTcpSocket *reverse_socket, QObject *parent);

    static IOTV_Client *client(std::unordered_map<IOTV_Client *, QThread *> &add_to_iot_client,
                                        uint maxClientCount,
                                        QTcpSocket *socket, QObject *parent);

    static IOTV_Host *host_broadcast(QUdpSocket *socket,
                                     std::unordered_map<IOTV_Host* , QThread*> &add_to_iot_hosts, uint maxHostCount,
                                     QObject *parent);

    static void bot(IOTV_Bot **_tg_bot, QThread **_tgbot_thread, const QString &token, const std::set<int64_t> &clients);

private:
    static void deleteReverseSocket(QAbstractSocket *reverse_socket);

//    QString correctNameSize(const QString &name) const;
};

