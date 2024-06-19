#pragma once

#include "IOTV_Host/iotv_host.h"
#include "IOTV_Client/iotv_client.h"

class Maker_iotv
{
public:
    Maker_iotv() = delete;

    static std::shared_ptr<IOTV_Host> host_tcp_in(IOTV_Host_List &add_to_iot_hosts, uint maxHostCount,
                           QTcpSocket *reverse_socket, QObject *parent);

    static std::shared_ptr<IOTV_Host> host(IOTV_Host_List &add_to_iot_hosts, uint maxHostCount,
                                    const std::unordered_map<QString, QString> &setting,
                                    QAbstractSocket *reverse_socket, QObject *parent);

    static std::shared_ptr<IOTV_Client> client(IOTV_Client_List &add_to_iot_client,
                                        uint maxClientCount,
                                        QTcpSocket *socket, QObject *parent);

    static std::shared_ptr<IOTV_Host> host_broadcast(std::shared_ptr<QUdpSocket> socket,
                                     IOTV_Host_List &add_to_iot_hosts, uint maxHostCount,
                                     QObject *parent);

private:
    static void deleteReverseSocket(QAbstractSocket *reverse_socket);

    QString correctNameSize(const QString &name) const;
};

