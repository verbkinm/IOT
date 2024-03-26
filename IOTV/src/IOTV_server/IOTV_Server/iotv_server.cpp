#include "iotv_server.h"

#include "event_action_parser.h"
#include "connection_type/tcp_conn_type.h"

#include <QNetworkDatagram>

IOTV_Server::IOTV_Server(QObject *parent) : QObject(parent),
    _settingsServer(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Server"),
    _settingsHosts(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Hosts"),
    _maxClientCount(1), _maxHostCount(10),
    _eventManager(nullptr)
{
    checkSettingsFileExist();
    readServerSettings();

    _tcpClient = new QTcpServer(this);
    _tcpReverseHost = new QTcpServer(this);
    _udpBroadcast = new QUdpSocket(this);

    Q_ASSERT(_tcpClient);
    Q_ASSERT(_tcpReverseHost);
    Q_ASSERT(_udpBroadcast);

    connect(_tcpClient, &QTcpServer::newConnection, this, &IOTV_Server::slotNewClientConnection);
    connect(_tcpReverseHost, &QTcpServer::newConnection, this, &IOTV_Server::slotNewHostConnection);
    connect(_udpBroadcast, &QUdpSocket::readyRead, this, &IOTV_Server::slotPendingDatagrams);

    startTCPServers();
    startUDPServers();

    readHostSetting();
    readEventActionJson();

    connect(&_reconnectTimer, &QTimer::timeout, this, &IOTV_Server::startTCPServers);
}

IOTV_Server::~IOTV_Server()
{
    for(auto &[client, thread] : _iot_clients)
    {
        thread->exit();
        thread->wait();
        thread->deleteLater();
    }

    _iot_clients.clear();

    for(auto &[hosts, thread] : _iot_hosts)
    {
        thread->exit();
        thread->wait();
        thread->deleteLater();
    }

    _iot_hosts.clear();


    while (_tcpClient->isListening())
    {
        _tcpClient->close();

        QThread::usleep(10);
    }
    Log::write("Stop TCP server.", Log::Write_Flag::FILE_STDOUT, ServerLog::TCP_LOG_FILENAME);
}

QStringList IOTV_Server::getFileSettingNames() const
{
    return {_settingsServer.fileName(), _settingsHosts.fileName()};
}

void IOTV_Server::readServerSettings()
{       
    _settingsServer.beginGroup("Server");
    _address = _settingsServer.value(serverField::address, "127.0.0.1").toString();
    _portForClients = _settingsServer.value(serverField::portClients, 2022).toUInt();
    _portTcpHosts = _settingsServer.value(serverField::portHosts, 2023).toUInt();
    _broadcasrListenerPort = _settingsServer.value(serverField::broadCastListenerPort, 2022).toUInt();
    _maxClientCount = _settingsServer.value(serverField::maxClient, 5).toUInt();
    _maxHostCount = _settingsServer.value(serverField::maxHost, 10).toUInt();
    ServerLog::TCP_LOG_FILENAME = _settingsServer.value(ServerLog::TCP_LOG, ServerLog::TCP_LOG_FILENAME).toString();
    ServerLog::CLIENT_ONLINE_LOG_FILENAME = _settingsServer.value(ServerLog::CLIENT_ONLINE_LOG, ServerLog::CLIENT_ONLINE_LOG_FILENAME).toString();
    ServerLog::DEFAULT_LOG_FILENAME = _settingsServer.value(ServerLog::DEFAULT_LOG, ServerLog::DEFAULT_LOG_FILENAME).toString();
    _settingsServer.endGroup();
}

IOTV_Host *IOTV_Server::createHost(std::unordered_map<QString, QString> &setting, QTcpSocket *reverse_socket)
{
    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&](const auto &pair){
        return pair.first->settingsData().at(hostField::name) == setting[hostField::name];
    });

    if (it != _iot_hosts.end())
    {
        Log::write(QString(Q_FUNC_INFO) + ", Error: Double host name in config file - " + setting[hostField::name],
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
    }

    QThread *th = new QThread(this);
    IOTV_Host *host = nullptr;

    if (setting[hostField::connection_type] == connectionType::TCP_REVERSE)
        host = new IOTV_Host(setting, reverse_socket, th);
    else
        host = new IOTV_Host(setting, th);

    th->start();
    _iot_hosts[host] = th;

    if (!th->isRunning())
    {
        Log::write(QString(Q_FUNC_INFO) + " Error: Can't run IOT_Host in new thread",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
    }

    return host;
}

void IOTV_Server::readHostSetting()
{
    for (const auto &group : _settingsHosts.childGroups())
    {
        // Лимит количества хостов
        if (_iot_hosts.size() == _maxHostCount)
        {
            Log::write(QString(Q_FUNC_INFO) +
                           ", Error: Hosts limit = " + QString::number(_maxHostCount),
                       Log::Write_Flag::FILE_STDERR,
                       ServerLog::DEFAULT_LOG_FILENAME);
            break;
        }

        _settingsHosts.beginGroup(group);
        std::unordered_map<QString, QString> setting;

        setting[hostField::name] = group.toLatin1();
        setting[hostField::name] = strlen(setting[hostField::name].toStdString().c_str()) > 30 ? QByteArray(setting[hostField::name].toStdString().c_str()).mid(0, 30) : setting[hostField::name];
        setting[hostField::connection_type] = _settingsHosts.value(hostField::connection_type, connectionType::TCP).toString();
        setting[hostField::address] = _settingsHosts.value(hostField::address, "127.0.0.1").toString();
        setting[hostField::interval] = _settingsHosts.value(hostField::interval, "1000").toString();
        setting[hostField::logFile] = _settingsHosts.value(hostField::logFile, setting[hostField::name] + ".log").toString();

        if (ipConnectionType(setting[hostField::connection_type]))
            setting[hostField::port] = _settingsHosts.value(hostField::port, "0").toString();

        //!!!
        //if (setting[hostField::connection_type] == connectionType::COM)
        //  ;

        createHost(setting, nullptr);

        _settingsHosts.endGroup();
    }
}

void IOTV_Server::readEventActionJson()
{
    QString fileName = QDir(QFileInfo(_settingsHosts.fileName()).path()).filePath(Json_Event_Action::EVENT_ACTION_FILE_NAME);
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    if (!file.isOpen())
    {
        file.open(QIODevice::WriteOnly);
        if (!file.isOpen())
        {
            Log::write("Can't create/open file: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
            exit(1);
        }
        file.write("{\n}");
        file.close();
        file.open(QIODevice::ReadOnly);

        // Если запись разрешена, а чтение всё же не доступно
        if (!file.isOpen())
        {
            Log::write("Can't create/open file: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
            exit(1);
        }
    }

    auto list = Event_Action_Parser::parseJson(file.readAll(), baseHostList());

    if (_eventManager != nullptr)
        delete _eventManager;

    _eventManager = new IOTV_Event_Manager(this);

    for(const auto &pairs : list)
        _eventManager->bind(pairs.first, pairs.second.first, pairs.second.second);
}

void IOTV_Server::writeEventActionJson(const QByteArray &data)
{
    QString fileName = QDir(QFileInfo(_settingsHosts.fileName()).path()).filePath(Json_Event_Action::EVENT_ACTION_FILE_NAME);
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    if (!file.isOpen())
    {
        Log::write("Can't create/open file: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
        exit(1);
    }

    file.write(data);
}

void IOTV_Server::startTCP(QTcpServer *socket, quint16 port, const QString &lbl)
{
    if (socket->isListening())
        return;

    if (socket->listen(QHostAddress(_address), port))
    {
        QString str = "Start TCP server for " + lbl + " " + _address + ":" + QString::number(port);
        Log::write(str, Log::Write_Flag::FILE_STDOUT, ServerLog::TCP_LOG_FILENAME);
    }
    else
    {
        QString str = "Error start TCP server for " + lbl + " " + _address + ":" + QString::number(port);
        Log::write(str, Log::Write_Flag::FILE_STDERR, ServerLog::TCP_LOG_FILENAME);
    }
}

void IOTV_Server::startUDP(QUdpSocket *socket, const QString &addr, quint16 port, const QString &lbl)
{
    if (socket->bind(QHostAddress(addr), port, QAbstractSocket::ReuseAddressHint))
    {
        QString str = "Start UDP server for " + lbl + addr + ":" + QString::number(port);
        Log::write(str, Log::Write_Flag::FILE_STDOUT, ServerLog::TCP_LOG_FILENAME);
    }
    else
    {
        QString str = "Error start UDP server for " + lbl + " " + addr + ":" + QString::number(port);;
        Log::write(str, Log::Write_Flag::FILE_STDERR, ServerLog::TCP_LOG_FILENAME);
    }
}

void IOTV_Server::startTCPServers()
{
    startTCP(_tcpClient, _portForClients, "clients");
    startTCP(_tcpReverseHost, _portTcpHosts, "hosts");

    if (_tcpClient->isListening() && _tcpReverseHost->isListening())
        _reconnectTimer.stop();
    else
        _reconnectTimer.start(TCP_conn_type::SERVER_RECONNECT_INTERVAL);
}

void IOTV_Server::startUDPServers()
{
    startUDP(_udpBroadcast, "255.255.255.255", _broadcasrListenerPort, "broadcast");
}

void IOTV_Server::clientOnlineFile() const
{
    std::ofstream file(ServerLog::CLIENT_ONLINE_LOG_FILENAME.toStdString().c_str(), std::ios::trunc);

    if (!file.is_open())
    {
        Log::write("Can't open " + ServerLog::CLIENT_ONLINE_LOG_FILENAME,
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    for(const auto &client : _iot_clients)
    {
        const QTcpSocket *socket = client.first->socket();
        file << socket->peerName().toStdString()
             << ": "
             << socket->peerAddress().toString().toStdString()
             << ':'
             << std::to_string(socket->peerPort())
             << '\n';
    }

    file.close();
}

Base_Host *IOTV_Server::baseHostFromName(const QString &name) const
{
    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&name](const auto &pair){
        return pair.first->getName() == name;
    });

    if (it != _iot_hosts.end())
        return it->first;

    return nullptr;
}

void IOTV_Server::clientHostsUpdate() const
{
    // Оповестить клиентов об обновлении устройств
    for (const auto &client : _iot_clients)
        emit client.first->signalUpdateHosts();
}

bool IOTV_Server::ipConnectionType(const QString &str) const
{
    if (str == connectionType::TCP || str == connectionType::UDP || str == connectionType::TCP_REVERSE)
        return true;

    return false;
}

std::forward_list<const Base_Host *> IOTV_Server::baseHostList() const
{
    std::forward_list<const Base_Host *> result;
    std::transform(_iot_hosts.begin(), _iot_hosts.end(), std::front_inserter(result), [](auto const &pair){
        return pair.first;
    });

    return result;
}

void IOTV_Server::checkSettingsFileExist()
{
    if (!QFileInfo::exists(_settingsServer.fileName()))
    {
        _settingsServer.beginGroup("Server");
        _settingsServer.setValue(serverField::address, "127.0.0.1");
        _settingsServer.setValue(serverField::portClients, 2022);
        _settingsServer.setValue(serverField::portHosts, 2023);
        _settingsServer.setValue(serverField::broadCastListenerPort, 2022);
        _settingsServer.setValue(serverField::maxClient, _maxClientCount);
        _settingsServer.setValue(serverField::maxHost, _maxHostCount);
        _settingsServer.setValue(ServerLog::TCP_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::TCP_LOG_FILENAME).absoluteFilePath());
        _settingsServer.setValue(ServerLog::CLIENT_ONLINE_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::CLIENT_ONLINE_LOG_FILENAME).absoluteFilePath());
        _settingsServer.setValue(ServerLog::DEFAULT_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::DEFAULT_LOG_FILENAME).absoluteFilePath());
        _settingsServer.endGroup();
        _settingsServer.sync();
    }
    if (!QFileInfo::exists(_settingsHosts.fileName()))
    {
        _settingsHosts.beginGroup("Default");
        _settingsHosts.setValue(hostField::connection_type, connectionType::TCP);
        _settingsHosts.setValue(hostField::address, "127.0.0.1");
        _settingsHosts.setValue(hostField::port, 2021);
        _settingsHosts.setValue(hostField::interval, 0);
        _settingsHosts.setValue(hostField::logFile, QFileInfo({QCoreApplication::applicationDirPath()}, "default.log").absoluteFilePath());
        _settingsHosts.endGroup();
        _settingsHosts.sync();
    }
}

void IOTV_Server::slotNewClientConnection()
{
    QTcpSocket* socket = _tcpClient->nextPendingConnection();

    if (!socket)
    {
        Log::write(QString(Q_FUNC_INFO) + " nextPendingConnection: ",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    Log::write("Client new connection: "
                   + socket->peerAddress().toString()
                   + ":"
                   + QString::number(socket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::TCP_LOG_FILENAME);

    if (_iot_clients.size() >= _maxClientCount)
    {
        Log::write("Warnning: exceeded the maximum client limit. Client disconnected.",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::TCP_LOG_FILENAME);
        socket->disconnectFromHost();
        socket->deleteLater();
        return;
    }

    QThread *th = new QThread(this);
    IOTV_Client *client = new IOTV_Client(socket, _iot_hosts, th);
    th->start();

    _iot_clients[client] = th;

    if (!th->isRunning())
    {
        Log::write(QString(Q_FUNC_INFO) + " Error: Can't run IOT_Client in new thread ",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    connect(client, &IOTV_Client::signalDisconnected, this, &IOTV_Server::slotClientDisconnected, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalFetchEventActionData, this, &IOTV_Server::slotFetchEventActionData, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalQueryEventActionData, this, &IOTV_Server::slotQueryEventActionData, Qt::QueuedConnection);

    clientOnlineFile();
}

void IOTV_Server::slotClientDisconnected()
{
    IOTV_Client* client = qobject_cast<IOTV_Client*>(sender());
    if (client == nullptr)
        return;

    QString strOut = "Client disconnected";
    Log::write(strOut, Log::Write_Flag::FILE_STDOUT, ServerLog::TCP_LOG_FILENAME);

    _iot_clients[client]->exit();
    _iot_clients[client]->wait();
    _iot_clients[client]->deleteLater();
    _iot_clients.erase(client);

    clientOnlineFile();
}

void IOTV_Server::slotNewHostConnection()
{
    QTcpSocket* socket = _tcpReverseHost->nextPendingConnection();

    if (!socket)
    {
        Log::write(QString(Q_FUNC_INFO) + " nextPendingConnection: ",
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    Log::write("Host new connection: "
                   + socket->peerAddress().toString()
                   + ":"
                   + QString::number(socket->peerPort()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::TCP_LOG_FILENAME);

    if (_iot_hosts.size() >= _maxHostCount)
    {
        Log::write("Warnning: exceeded the maximum host limit. Host disconnected.",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::TCP_LOG_FILENAME);
        socket->disconnectFromHost();
        socket->deleteLater();
        return;
    }

    std::unordered_map<QString, QString> setting;
    setting[hostField::connection_type] = connectionType::TCP_REVERSE;
    setting[hostField::address] = socket->peerAddress().toString();
    setting[hostField::port] = QString::number(socket->peerPort());
    setting[hostField::interval] = "1000";
    setting[hostField::logFile] = setting[hostField::address] + ":" + setting[hostField::port] + ".log";
    setting[hostField::name] = socket->peerName() + ":" + setting[hostField::address] + ":" + setting[hostField::port];

    auto host = createHost(setting, socket);

    connect(host, &IOTV_Host::signalDevicePingTimeOut, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);
    connect(host, &IOTV_Host::signalDisconnected, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);

    // Оповестить клиентов об обновлении устройств
    clientHostsUpdate();
}


void IOTV_Server::slotError(QAbstractSocket::SocketError error)
{
    QString strErr;
    switch (error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        strErr = "The connection was refused by the peer (or timed out).";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        strErr = "The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.";
        break;
    case QAbstractSocket::HostNotFoundError:
        break;
    case QAbstractSocket::SocketAccessError:
        break;
    case QAbstractSocket::SocketResourceError:
        break;
    case QAbstractSocket::SocketTimeoutError:
        break;
    case QAbstractSocket::DatagramTooLargeError:
        break;
    case QAbstractSocket::NetworkError:
        break;
    case QAbstractSocket::AddressInUseError:
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        break;
    case QAbstractSocket::ProxyNotFoundError:
        break;
    case QAbstractSocket::ProxyProtocolError:
        break;
    case QAbstractSocket::OperationError:
        break;
    case QAbstractSocket::SslInternalError:
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        break;
    case QAbstractSocket::TemporaryError:
        break;
    case QAbstractSocket::UnknownSocketError:
        strErr = "UnknownSocketError";
        break;
    }

    Log::write(this->objectName() + ": " + strErr,
               Log::Write_Flag::FILE_STDERR,
               ServerLog::TCP_LOG_FILENAME);

    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}

void IOTV_Server::slotFetchEventActionData(QByteArray data)
{
    QJsonParseError err;
    QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parse json " << err.errorString() << ' ' << err.offset;
        exit(-1);
    }

    writeEventActionJson(data);
    readEventActionJson();
}

void IOTV_Server::slotQueryEventActionData()
{
    IOTV_Client *client = dynamic_cast<IOTV_Client *>(sender());

    if (client == nullptr || _eventManager == nullptr)
        return;

    QByteArray data = Event_Action_Parser::toData(_eventManager->worker());
    emit client->signalFetchEventActionDataFromServer(data);
}

void IOTV_Server::slotPendingDatagrams()
{
    QByteArray data;
    QNetworkDatagram dataGram;

    if (_udpBroadcast->hasPendingDatagrams())
    {
        dataGram = _udpBroadcast->receiveDatagram();
        data = dataGram.data();
    }

    bool error;
    uint64_t cutDataSize, expectedDataSize;

    struct Header *header = createPkgs(reinterpret_cast<uint8_t *>(data.data()), data.size(), &error, &expectedDataSize, &cutDataSize);

    if (error == true)
    {
        clearHeader(header);
        qDebug() << "datagram error = ";
        return;
    }

    // Пакет не ещё полный
    if (expectedDataSize > 0)
    {
        clearHeader(header);
        qDebug() << "datagram expectedDataSize = " << expectedDataSize;
        return;
    }

    if (header->assignment == HEADER_ASSIGNMENT_HOST_BROADCAST)
    {
        const struct Host_Broadcast *hb = static_cast<const struct Host_Broadcast *>(header->pkg);

        // Лимит количества хостов
        if (_iot_hosts.size() == _maxHostCount)
        {
            Log::write(QString(Q_FUNC_INFO) +
                           ", Error: Hosts limit = " + QString::number(_maxHostCount),
                       Log::Write_Flag::FILE_STDERR,
                       ServerLog::DEFAULT_LOG_FILENAME);
            return;
        }

        std::unordered_map<QString, QString> setting;

        setting[hostField::name] = QString(QByteArray(hb->name, hb->nameSize)).toLatin1();
        setting[hostField::name] = strlen(setting[hostField::name].toStdString().c_str()) > 30 ? QByteArray(setting[hostField::name].toStdString().c_str()).mid(0, 30) : setting[hostField::name];
        setting[hostField::name] += " " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");

        switch(hb->flags)
        {
        case Host_Broadcast_FLAGS_UDP_CONN:
            setting[hostField::connection_type] = connectionType::UDP;
            break;
        case Host_Broadcast_FLAGS_TCP_CONN:
        default:
            setting[hostField::connection_type] = connectionType::TCP;
        }

        setting[hostField::address] = QHostAddress(hb->address).toString();
        setting[hostField::interval] = "1000";
        setting[hostField::logFile] = hostField::logFile + setting[hostField::name] + ".log";

        if (ipConnectionType(setting[hostField::connection_type]))
            setting[hostField::port] = QString::number(hb->port);

        clearHeader(header);

        auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&setting](const auto &pair){
            return (pair.first->settingsData().at(hostField::address) == setting[hostField::address] &&
                    pair.first->settingsData().at(hostField::port) == setting[hostField::port]);
        });

        if (it != _iot_hosts.end())
        {
            Log::write(QString(Q_FUNC_INFO) +
                           ", Error: Double address and port from broadcast - " + setting[hostField::name],
                       Log::Write_Flag::FILE_STDERR,
                       ServerLog::DEFAULT_LOG_FILENAME);
            return;
        }

        QThread *th = new QThread(this);
        IOTV_Host *host = new IOTV_Host(setting, th);

        th->start();
        _iot_hosts[host] = th;

        if (!th->isRunning())
        {
            Log::write(QString(Q_FUNC_INFO) +
                           " Error: Can't run IOT_Host in new thread",
                       Log::Write_Flag::FILE_STDOUT,
                       ServerLog::DEFAULT_LOG_FILENAME);
            //            exit(1);
            return;
        }

        connect(host, &IOTV_Host::signalDevicePingTimeOut, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);

        // Оповестить клиентов об обновлении устройств
        clientHostsUpdate();
    }
    else
    {
        Log::write("datagram назначение пакета не определено!",
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG_FILENAME);
    }
}

void IOTV_Server::slotDevicePingTimeout()
{
    IOTV_Host *host = dynamic_cast<IOTV_Host *>(sender());

    if (host == nullptr)
        return;

    _iot_hosts[host]->exit();
    _iot_hosts[host]->wait();
    _iot_hosts[host]->deleteLater();
    _iot_hosts.erase(host);

    for (const auto &client : _iot_clients)
        emit client.first->signalClearAndUpdateHosts();
}

void IOTV_Server::slotTest()
{
    // TESTING
}
