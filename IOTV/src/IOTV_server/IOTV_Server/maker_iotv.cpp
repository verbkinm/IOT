#include "maker_iotv.h"

#include <QThread>
#include <QFileInfo>
#include <QDir>

void Maker_iotv::deleteReverseSocket(QAbstractSocket *reverse_socket)
{
    if (reverse_socket)
    {
        reverse_socket->abort();
        reverse_socket->deleteLater();
    }
}

std::shared_ptr<IOTV_Host> Maker_iotv::host_tcp_in(IOTV_Host_List &add_to_iot_hosts, uint maxHostCount, QTcpSocket *reverse_socket, QObject *parent)
{
    if (!reverse_socket)
    {
        Log::write(CATEGORY::WARNING, QString(Q_FUNC_INFO) + " nextPendingConnection: ",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return nullptr;
    }

    Log::write(CATEGORY::NET, "Host new connection: "
                   + reverse_socket->peerAddress().toString()
                   + ":"
                   + QString::number(reverse_socket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    if (add_to_iot_hosts.size() >= maxHostCount)
    {
        Log::write(CATEGORY::WARNING, "exceeded the maximum host limit. Host disconnected.",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
        reverse_socket->disconnectFromHost();
        reverse_socket->deleteLater();
        return nullptr;
    }

    std::unordered_map<QString, QString> setting;
    setting[hostField::connection_type] = connectionType::TCP_REVERSE;
    setting[hostField::address] = reverse_socket->peerAddress().toString();
    setting[hostField::port] = QString::number(reverse_socket->peerPort());
    setting[hostField::interval] = "1000";
    setting[hostField::name] = reverse_socket->peerName() + ":" + setting[hostField::address] + ":" + setting[hostField::port];
    setting[hostField::logDir] = setting[hostField::address] + ":" + setting[hostField::port];

    return Maker_iotv::host(add_to_iot_hosts, maxHostCount, setting, reverse_socket, parent);
}

std::shared_ptr<IOTV_Host> Maker_iotv::host(IOTV_Host_List &add_to_iot_hosts,
                            uint maxHostCount,
                            const std::unordered_map<QString, QString> &setting,
                            QAbstractSocket *reverse_socket, QObject *parent)
{
    if (add_to_iot_hosts.size() == maxHostCount)
    {
        Log::write(CATEGORY::WARNING, QString(Q_FUNC_INFO) +
                       "Hosts limit = " + QString::number(maxHostCount),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);

        deleteReverseSocket(reverse_socket);

        return nullptr;
    }

    auto it = std::find_if (add_to_iot_hosts.begin(), add_to_iot_hosts.end(), [&](const auto &host){
        return host->settingsData().at(hostField::name) == setting.at(hostField::name);
    });

    if (it != add_to_iot_hosts.end())
    {
        Log::write(CATEGORY::ERROR, QString(Q_FUNC_INFO) + "Double host name in config file - " + setting.at(hostField::name),
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);

        deleteReverseSocket(reverse_socket);

        return nullptr;
    }

    QThread *th = new QThread;
    std::shared_ptr<IOTV_Host> host;

    if (setting.at(hostField::connection_type) == connectionType::TCP_REVERSE)
    {
        auto socket = dynamic_cast<QTcpSocket *>(reverse_socket);
        if (socket == nullptr)
        {
            Log::write(CATEGORY::ERROR, QString(Q_FUNC_INFO) + "Error dynamic_cast!",
                       Log::Write_Flag::FILE_STDERR,
                       ServerLog::DEFAULT_LOG_FILENAME);

            delete th;
            return nullptr;
        }

        host = std::make_shared<IOTV_Host>(setting, socket);
    }
    else
        host = std::make_shared<IOTV_Host>(setting);

    host->moveToThread(th);

    th->start();

    if (!th->isRunning())
    {
        Log::write(CATEGORY::ERROR, QString(Q_FUNC_INFO) + "Can't run IOT_Host in new thread",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
        delete th;

        return nullptr;
    }
    add_to_iot_hosts.insert(host);


    return host;
}

std::shared_ptr<IOTV_Client> Maker_iotv::client(IOTV_Client_List &add_to_iot_client, uint maxClientCount,
                                QTcpSocket *socket, QObject *parent)
{
    if (!socket)
    {
        Log::write(CATEGORY::WARNING, QString(Q_FUNC_INFO) + " nextPendingConnection: ",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return nullptr;
    }

    Log::write(CATEGORY::NET, "Client new connection: "
                   + socket->peerAddress().toString()
                   + ":" + QString::number(socket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    if (add_to_iot_client.size() >= maxClientCount)
    {
        Log::write(CATEGORY::WARNING, "превышет лимит клиентов. Новый клиент отключен.",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
        socket->disconnectFromHost();
        socket->deleteLater();
        return nullptr;
    }

    QThread *th = new QThread;
    th->start();

    if (!th->isRunning())
    {
        Log::write(CATEGORY::ERROR, QString(Q_FUNC_INFO) + "невозможно запустить IOTV_Client в новом потоке!",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);

        delete th;

        return nullptr;
    }

    std::shared_ptr<IOTV_Client> client = std::make_shared<IOTV_Client>(socket);
    client.get()->moveToThread(th);

//    qDebug() << client->thread();
//    th->exit();
//    th->wait();
//    delete th;
//    qDebug() << client->thread();

    add_to_iot_client.insert(client);

    return client;
}

std::shared_ptr<IOTV_Host> Maker_iotv::host_broadcast(std::shared_ptr<QUdpSocket> socket,
                                      IOTV_Host_List &add_to_iot_hosts,
                                      uint maxHostCount,
                                      QObject *parent)
{
    if (!socket->hasPendingDatagrams())
        return nullptr;

    std::shared_ptr<IOTV_Host> host;
    QNetworkDatagram dataGram = socket->receiveDatagram();
    QByteArray data = dataGram.data();

    bool error;
    uint64_t cutDataSize, expectedDataSize;

    header_t *header = createPkgs(reinterpret_cast<uint8_t *>(data.data()), data.size(), &error, &expectedDataSize, &cutDataSize);

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
            Log::write(CATEGORY::WARNING, QString(Q_FUNC_INFO) +
                           "Hosts limit = " + QString::number(maxHostCount),
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
            Log::write(CATEGORY::ERROR, QString(Q_FUNC_INFO) + "Error flag!",
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

        setting[hostField::logDir] = setting[hostField::name];

        clearHeader(header);

        host = Maker_iotv::host(add_to_iot_hosts, maxHostCount, setting, nullptr, parent);
    }
    else
    {
        Log::write(CATEGORY::WARNING, "datagram назначение пакета не определено!",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
    }

    return host;
}
