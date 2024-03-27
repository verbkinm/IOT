#include "maker_iotv.h"

#include <QThread>

void Maker_iotv::deleteReverseSocket(QAbstractSocket *reverse_socket)
{
    if (reverse_socket)
    {
        reverse_socket->abort();
        reverse_socket->deleteLater();
    }
}

IOTV_Host *Maker_iotv::host_tcp_in(std::unordered_map<IOTV_Host *, QThread *> &add_to_iot_hosts, uint maxHostCount, QTcpSocket *reverse_socket, QObject *parent)
{
    if (!reverse_socket)
    {
        Log::write(QString(Q_FUNC_INFO) + " nextPendingConnection: ",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return nullptr;
    }

    Log::write("Host new connection: "
                   + reverse_socket->peerAddress().toString()
                   + ":"
                   + QString::number(reverse_socket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::TCP_LOG_FILENAME);

    if (add_to_iot_hosts.size() >= maxHostCount)
    {
        Log::write("Warnning: exceeded the maximum host limit. Host disconnected.",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::TCP_LOG_FILENAME);
        reverse_socket->disconnectFromHost();
        reverse_socket->deleteLater();
        return nullptr;
    }

    std::unordered_map<QString, QString> setting;
    setting[hostField::connection_type] = connectionType::TCP_REVERSE;
    setting[hostField::address] = reverse_socket->peerAddress().toString();
    setting[hostField::port] = QString::number(reverse_socket->peerPort());
    setting[hostField::interval] = "1000";
    setting[hostField::logFile] = setting[hostField::address] + ":" + setting[hostField::port] + ".log";
    setting[hostField::name] = reverse_socket->peerName() + ":" + setting[hostField::address] + ":" + setting[hostField::port];

    return Maker_iotv::host(add_to_iot_hosts, maxHostCount, setting, reverse_socket, parent);
}

IOTV_Host *Maker_iotv::host(std::unordered_map<IOTV_Host *, QThread *> &add_to_iot_hosts,
                            uint maxHostCount,
                            const std::unordered_map<QString, QString> &setting,
                            QAbstractSocket *reverse_socket, QObject *parent)
{
    if (add_to_iot_hosts.size() == maxHostCount)
    {
        Log::write(QString(Q_FUNC_INFO) +
                       ", Error: Hosts limit = " + QString::number(maxHostCount),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);

        deleteReverseSocket(reverse_socket);

        return nullptr;
    }

    auto it = std::find_if (add_to_iot_hosts.begin(), add_to_iot_hosts.end(), [&](const auto &pair){
        return pair.first->settingsData().at(hostField::name) == setting.at(hostField::name);
    });

    if (it != add_to_iot_hosts.end())
    {
        Log::write(QString(Q_FUNC_INFO) + ", Error: Double host name in config file - " + setting.at(hostField::name),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);

        deleteReverseSocket(reverse_socket);

        return nullptr;
    }

    QThread *th = new QThread(parent);
    IOTV_Host *host = nullptr;

    if (setting.at(hostField::connection_type) == connectionType::TCP_REVERSE)
    {
        auto socket = dynamic_cast<QTcpSocket *>(reverse_socket);
        if (socket == nullptr)
        {
            Log::write(QString(Q_FUNC_INFO) + " Error dynamic_cast!",
                       Log::Write_Flag::FILE_STDERR,
                       ServerLog::DEFAULT_LOG_FILENAME);

            delete th;
            return nullptr;
        }

        host = new IOTV_Host(setting, socket, th);
    }
    else
        host = new IOTV_Host(setting, th);

    th->start();
    add_to_iot_hosts[host] = th;

    if (!th->isRunning())
    {
        Log::write(QString(Q_FUNC_INFO) + " Error: Can't run IOT_Host in new thread",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);

        add_to_iot_hosts.erase(host);
        delete th;

        return nullptr;
    }

    return host;
}

IOTV_Client *Maker_iotv::client(std::unordered_map<IOTV_Client *, QThread *> &add_to_iot_client, uint maxClientCount,
                                const std::unordered_map<IOTV_Host *, QThread *> &iot_hosts,
                                QTcpSocket *socket, QObject *parent)
{
    if (!socket)
    {
        Log::write(QString(Q_FUNC_INFO) + " nextPendingConnection: ",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return nullptr;
    }

    Log::write("Client new connection: "
                   + socket->peerAddress().toString()
                   + ":" + QString::number(socket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::TCP_LOG_FILENAME);

    if (add_to_iot_client.size() >= maxClientCount)
    {
        Log::write("Внимание: превышет лимит клиентов. Новый клиент отключен.",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::TCP_LOG_FILENAME);
        socket->disconnectFromHost();
        socket->deleteLater();
        return nullptr;
    }

    QThread *th = new QThread(parent);
    IOTV_Client *client = new IOTV_Client(socket, iot_hosts, th);
    th->start();

    add_to_iot_client[client] = th;

    if (!th->isRunning())
    {
        Log::write(QString(Q_FUNC_INFO) + " Error: Can't run IOT_Client in new thread ",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return nullptr;
    }

    return client;
}

IOTV_Host *Maker_iotv::host_broadcast(QUdpSocket *socket,
                                      std::unordered_map<IOTV_Host *, QThread *> &add_to_iot_hosts,
                                      uint maxHostCount,
                                      QObject *parent)
{
    if (!socket->hasPendingDatagrams())
        return nullptr;

    IOTV_Host *host = nullptr;
    QNetworkDatagram dataGram = socket->receiveDatagram();
    QByteArray data = dataGram.data();

    bool error;
    uint64_t cutDataSize, expectedDataSize;

    struct Header *header = createPkgs(reinterpret_cast<uint8_t *>(data.data()), data.size(), &error, &expectedDataSize, &cutDataSize);

    if (error == true)
    {
        clearHeader(header);
        qDebug() << "datagram error = ";
        return nullptr;
    }

    // Пакет не ещё полный
    if (expectedDataSize > 0)
    {
        clearHeader(header);
        qDebug() << "datagram expectedDataSize = " << expectedDataSize;
        return nullptr;
    }

    if (header->assignment == HEADER_ASSIGNMENT_HOST_BROADCAST)
    {
        const struct Host_Broadcast *hb = static_cast<const struct Host_Broadcast *>(header->pkg);

        // Лимит количества хостов
        if (add_to_iot_hosts.size() == maxHostCount)
        {
            Log::write(QString(Q_FUNC_INFO) +
                           ", Error: Hosts limit = " + QString::number(maxHostCount),
                       Log::Write_Flag::FILE_STDERR,
                       ServerLog::DEFAULT_LOG_FILENAME);
            return nullptr;
        }

        std::unordered_map<QString, QString> setting;

        switch(hb->flags)
        {
        case Host_Broadcast_FLAGS_UDP_CONN:
            setting[hostField::connection_type] = connectionType::UDP;
            break;
        case Host_Broadcast_FLAGS_TCP_CONN:
            setting[hostField::connection_type] = connectionType::TCP;
            break;
        case Host_Broadcast_FLAGS_ERROR:
        default:
            Log::write(QString(Q_FUNC_INFO) + ", Error flag!",
                       Log::Write_Flag::FILE_STDERR,
                       ServerLog::DEFAULT_LOG_FILENAME);
            return nullptr;
        }

        setting[hostField::address] = QHostAddress(hb->address).toString();
        setting[hostField::interval] = "1000";
        setting[hostField::name] = QByteArray(hb->name, hb->nameSize)  + " " + setting[hostField::address];

        if (Base_conn_type::isIpConnectionType(setting[hostField::connection_type]))
        {
            setting[hostField::port] = QString::number(hb->port);
            setting[hostField::name] += ":" + setting[hostField::port];
        }

        setting[hostField::logFile] = hostField::logFile + setting[hostField::name] + ".log";

        clearHeader(header);

        host = Maker_iotv::host(add_to_iot_hosts, maxHostCount, setting, nullptr, parent);
    }
    else
    {
        Log::write("datagram назначение пакета не определено!",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
    }

    return host;
}
