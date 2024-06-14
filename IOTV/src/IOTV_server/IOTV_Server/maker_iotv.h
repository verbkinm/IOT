#pragma once

#include "IOTV_Host/iotv_host.h"
#include "IOTV_Client/iotv_client.h"

class Maker_iotv
{
public:
    Maker_iotv() = delete;

    static IOTV_Host *host_tcp_in(std::unordered_map<IOTV_Host* , QThread*> &add_to_iot_hosts, uint maxHostCount,
                           QTcpSocket *reverse_socket, QObject *parent);

    static IOTV_Host *host(std::unordered_map<IOTV_Host* , QThread*> &add_to_iot_hosts, uint maxHostCount,
                                    const std::unordered_map<QString, QString> &setting,
                                    QAbstractSocket *reverse_socket, QObject *parent);

    static IOTV_Client *client(std::unordered_map<IOTV_Client*, QThread*> &add_to_iot_client,
                                        uint maxClientCount,
                                        QTcpSocket *socket, QObject *parent);

    static IOTV_Host *host_broadcast(QUdpSocket *socket,
                                     std::unordered_map<IOTV_Host* , QThread*> &add_to_iot_hosts, uint maxHostCount,
                                     QObject *parent);

private:
    static void deleteReverseSocket(QAbstractSocket *reverse_socket);

    QString correctNameSize(const QString &name) const;
};

