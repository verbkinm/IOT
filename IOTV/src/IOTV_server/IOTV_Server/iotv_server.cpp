#include "iotv_server.h"

#include "event_action_parser.h"
#include "connection_type/tcp_conn_type.h"
#include "maker_iotv.h"

#include <QNetworkDatagram>

IOTV_Server::IOTV_Server(QObject *parent) : QObject(parent),
    _settingsServer(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Server"),
    _settingsHosts(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Hosts"),
    _maxClientCount(1), _maxHostCount(10)
{
    checkSettingsFileExist();
    readServerSettings();

    _tcpClient = std::make_shared<QTcpServer>(this);
    _tcpReverseHost = std::make_shared<QTcpServer>(this);
    _udpBroadcast = std::make_shared<QUdpSocket>(this);

    Q_ASSERT(_tcpClient);
    Q_ASSERT(_tcpReverseHost);
    Q_ASSERT(_udpBroadcast);

    connect(_tcpClient.get(), &QTcpServer::newConnection, this, &IOTV_Server::slotNewClientConnection);
    connect(_tcpReverseHost.get(), &QTcpServer::newConnection, this, &IOTV_Server::slotNewHostConnection);
    connect(_udpBroadcast.get(), &QUdpSocket::readyRead, this, &IOTV_Server::slotPendingDatagrams);

    startTCPServers();
    startUDPServers();

    readHostSetting();
    readEventActionJson();

    connect(&_reconnectTimer, &QTimer::timeout, this, &IOTV_Server::startTCPServers);

    connect(this, &IOTV_Server::signalDestroy, this, &IOTV_Server::slotDestroy, Qt::QueuedConnection);

    clientOnlineFile();
}

IOTV_Server::~IOTV_Server()
{
    qDebug() << "IOTV_Server destruct";

//    for(auto &client : _iot_clients)
//        emit client->signalMoveToThread(QThread::currentThread());

//    for(auto &host : _iot_hosts)
//        emit host->signalMoveToThread(QThread::currentThread());


//    // TCP сервер
//    while (_tcpClient->isListening())
//    {
//        _tcpClient->close();
//        QThread::usleep(10);
//    }

//    Log::write(CATEGORY::NET, "Stop TCP server.", Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);

//    _tgbot_thread->exit();
//    _tgbot_thread->wait();
}

QStringList IOTV_Server::getFileSettingNames() const
{
    return {_settingsServer.fileName(), _settingsHosts.fileName()};
}

void IOTV_Server::readServerSettings()
{
    _settingsServer.beginGroup(serverField::SERVER_GROUP);
    _address = _settingsServer.value(serverField::address, "127.0.0.1").toString();
    _portForClients = _settingsServer.value(serverField::portClients, 2022).toUInt();
    _portTcpHosts = _settingsServer.value(serverField::portHosts, 2023).toUInt();
    _broadcasrListenerPort = _settingsServer.value(serverField::broadCastListenerPort, 2022).toUInt();
    _maxClientCount = _settingsServer.value(serverField::maxClient, 5).toUInt();
    _maxHostCount = _settingsServer.value(serverField::maxHost, 10).toUInt();
    ServerLog::CLIENT_ONLINE_LOG_FILENAME = _settingsServer.value(ServerLog::CLIENT_ONLINE_LOG, ServerLog::CLIENT_ONLINE_LOG_FILENAME).toString();
    ServerLog::EVENT_ACTION_LOG_FILENAME = _settingsServer.value(ServerLog::EVENT_ACTION_LOG, ServerLog::EVENT_ACTION_LOG_FILENAME).toString();
    ServerLog::DEFAULT_LOG_FILENAME = _settingsServer.value(ServerLog::DEFAULT_LOG, ServerLog::DEFAULT_LOG_FILENAME).toString();
    _settingsServer.endGroup();

    _settingsServer.beginGroup(tgBotField::TG_BOT_GROUP);
    tgBotFileName::TGBOT_LOG_FILENAME = _settingsServer.value(tgBotFileName::TGBOT_LOG_FILENAME, tgBotFileName::TGBOT_LOG_FILENAME).toString();
    tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME = _settingsServer.value(tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME, tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME).toString();

    QString token = _settingsServer.value(tgBotField::TGBOTTOKEN, "").toString();
    std::set<int64_t> clients;

    std::ifstream file(tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME.toStdString());
    if (!file.is_open())
        Log::write(CATEGORY::ERROR, QString("Не удалось открыть файл %1").arg(tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME), Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
    else
    {
        while(!file.eof())
        {
            int64_t chatId;
            file >> chatId;
            clients.insert(chatId);
        }
    }

//    if (token != "")
//    {
//        _tgbot_thread.reset();
//        _tgbot_thread = std::make_shared<QThread>(this);

//        _tg_bot.reset();
//        _tg_bot = std::make_shared<IOTV_Bot>(token, clients);
//        _tg_bot->moveToThread(_tgbot_thread.get());

//        _tgbot_thread->start();

//        if (!_tgbot_thread->isRunning())
//        {
//            Log::write(CATEGORY::ERROR, QString(Q_FUNC_INFO) + "невозможно запустить IOTV_Bot в новом потоке!",
//                       Log::Write_Flag::FILE_STDERR,
//                       ServerLog::DEFAULT_LOG_FILENAME);
//            _tg_bot.reset();
//            _tg_bot.reset();
//        }
//        else
//        {
//            connect(_tg_bot.get(), &IOTV_Bot::signalBotRequest, this, &IOTV_Server::slotBotRequest, Qt::QueuedConnection);
//            emit _tg_bot->signalStart();
//        }
//    }
    _settingsServer.endGroup();
}

void IOTV_Server::readHostSetting()
{
    for (const auto &group : _settingsHosts.childGroups())
    {
        _settingsHosts.beginGroup(group);
        std::unordered_map<QString, QString> setting;

        setting[hostField::name] = group;
        setting[hostField::connection_type] = _settingsHosts.value(hostField::connection_type, connectionType::TCP).toString();
        setting[hostField::address] = _settingsHosts.value(hostField::address, "127.0.0.1").toString();
        setting[hostField::interval] = _settingsHosts.value(hostField::interval, "1000").toString();
        setting[hostField::logDir] = _settingsHosts.value(hostField::logDir, setting[hostField::name]).toString();

        if (Base_conn_type::isIpConnectionType(setting[hostField::connection_type]))
            setting[hostField::port] = _settingsHosts.value(hostField::port, "0").toString();

        //!!!
        //if (setting[hostField::connection_type] == connectionType::COM)
        //  ;

        auto host = Maker_iotv::host(_iot_hosts, _maxHostCount, setting, nullptr, this);
        if (host != nullptr)
        {
            connect(host.get(), &IOTV_Host::signalIdentRecived, this, &IOTV_Server::clientHostsUpdate, Qt::QueuedConnection);
            connect(host.get(), &IOTV_Host::signalMovedToThread, this, &IOTV_Server::slotClearHostObj, Qt::QueuedConnection);
        }

        _settingsHosts.endGroup();
    }
}

QByteArray IOTV_Server::readEventActionJson()
{
    QString fileName = QDir(QFileInfo(_settingsHosts.fileName()).path()).filePath(Json_Event_Action::EVENT_ACTION_FILE_NAME);
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    if (!file.isOpen())
    {
        file.open(QIODevice::WriteOnly);
        if (!file.isOpen())
        {
            Log::write(CATEGORY::ERROR, "Can't create/open file: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
            return {};
        }
        file.write("{\n}\n");
        file.close();
        file.open(QIODevice::ReadOnly);

        // Если запись разрешена, а чтение всё же не доступно
        if (!file.isOpen())
        {
            Log::write(CATEGORY::ERROR, "Can't create/open file: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
            return {};
        }
    }

    QByteArray result = file.readAll();

    std::forward_list<const Base_Host *> hosts;
    for (const auto &host : _iot_hosts)
        hosts.push_front(host.get());

    auto pairs = Event_Action_Parser::parseJson(result, hosts);
    auto event_vec = pairs.first.first;
    auto action_vec = pairs.first.second;
    auto event_groups = pairs.second.first;
    auto action_groups = pairs.second.second;

    _eventManager.reset();
    _eventManager = std::make_unique<IOTV_Event_Manager>(event_vec, action_vec, event_groups, action_groups);
    _eventManager->bind();


    for (const auto &event : _eventManager->events())
        connect(event.get(), &IOTV_Event::signalEvent, this, &IOTV_Server::slotEvent);

    for (const auto &action : _eventManager->actions())
        connect(action.get(), &IOTV_Action::signalAction, this, &IOTV_Server::slotAction);

    return result;
}

void IOTV_Server::writeEventActionJson(const QByteArray &data)
{
    QString fileName = QDir(QFileInfo(_settingsHosts.fileName()).path()).filePath(Json_Event_Action::EVENT_ACTION_FILE_NAME);
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    if (!file.isOpen())
    {
        Log::write(CATEGORY::ERROR, "Can't create/open file: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
        exit(1);
    }

    file.write(data);
}

void IOTV_Server::startTCP(std::shared_ptr<QTcpServer> socket, quint16 port, const QString &lbl)
{
    if (socket->isListening())
        return;

    if (socket->listen(QHostAddress(_address), port))
    {
        QString str = "Start TCP server for " + lbl + " " + _address + ":" + QString::number(port);
        Log::write(CATEGORY::NET, str, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
    }
    else
    {
        QString str = "Error start TCP server for " + lbl + " " + _address + ":" + QString::number(port);
        Log::write(CATEGORY::ERROR, str, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
    }
}

void IOTV_Server::startUDP(std::shared_ptr<QUdpSocket> socket, const QString &addr, quint16 port, const QString &lbl)
{
    if (socket->bind(QHostAddress(addr), port, QAbstractSocket::ReuseAddressHint))
    {
        QString str = "Start UDP server for " + lbl + addr + ":" + QString::number(port);
        Log::write(CATEGORY::NET, str, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
    }
    else
    {
        QString str = "Error start UDP server for " + lbl + " " + addr + ":" + QString::number(port);;
        Log::write(CATEGORY::ERROR, str, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
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
    startUDP(_udpBroadcast, "255.255.255.255", _broadcasrListenerPort, "broadcast ");
}

void IOTV_Server::clientOnlineFile() const
{
    std::ofstream file(ServerLog::CLIENT_ONLINE_LOG_FILENAME.toStdString().c_str(), std::ios::trunc);

    if (!file.is_open())
    {
        Log::write(CATEGORY::ERROR, "Can't open " + ServerLog::CLIENT_ONLINE_LOG_FILENAME,
                   Log::Write_Flag::FILE_STDERR,
                   ServerLog::DEFAULT_LOG_FILENAME);
        return;
    }

    for(const auto &client : _iot_clients)
    {
        const QTcpSocket *socket = client->socket();
        file << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz - ").toStdString().c_str()
             << socket->peerName().toStdString()
             << ": "
             << socket->peerAddress().toString().toStdString()
             << ':'
             << std::to_string(socket->peerPort())
             << '\n';
    }

    file.close();
}

void IOTV_Server::clientHostsUpdate()
{
    IOTV_Host *host = dynamic_cast<IOTV_Host *>(sender());

    if (host != nullptr)
    // Если хост поменял имя и оно не уникально, добавляем к имени номер
    {
        QString name = host->getName();
        int countName = 0;

        for (const auto &host : _iot_hosts)
        {
            if (host->getName() == name)
                ++countName;
        }

        if (countName > 1)
            host->setName(name + "_" + QString::number(countName));
    }

    char outData[BUFSIZ];
    QByteArray result;

    if (_iot_hosts.size() == 0)
    {
        auto size = responseIdentificationData(outData, BUFSIZ, NULL, Identification_FLAGS_NONE);
        result = QByteArray(outData, static_cast<int>(size));
    }
    else
    {
        for (const auto &host : _iot_hosts)
        {
            iotv_obj_t *iot = host->convert();
            auto size = responseIdentificationData(outData, BUFSIZ, iot, Identification_FLAGS_NONE);

            result += QByteArray{outData, static_cast<int>(size)};
            clear_iotv_obj(iot);
        }
    }

    readEventActionJson();

    for (const auto &client : _iot_clients)
        emit client->signalUpdateHosts(result);
}

//std::forward_list<const Base_Host *> IOTV_Server::baseHostList() const
//{
//    std::forward_list<const Base_Host *> result;
//    std::transform(_iot_hosts.begin(), _iot_hosts.end(), std::front_inserter(result), [](auto const &host){
//        return host;
//    });

//    return result;
//}

void IOTV_Server::checkSettingsFileExist()
{
    if (!QFileInfo::exists(_settingsServer.fileName()))
    {
        _settingsServer.beginGroup(serverField::SERVER_GROUP);
        _settingsServer.setValue(serverField::address, "127.0.0.1");
        _settingsServer.setValue(serverField::portClients, 2022);
        _settingsServer.setValue(serverField::portHosts, 2023);
        _settingsServer.setValue(serverField::broadCastListenerPort, 2022);
        _settingsServer.setValue(serverField::maxClient, _maxClientCount);
        _settingsServer.setValue(serverField::maxHost, _maxHostCount);
        _settingsServer.setValue(ServerLog::CLIENT_ONLINE_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::CLIENT_ONLINE_LOG_FILENAME).absoluteFilePath());
        _settingsServer.setValue(ServerLog::EVENT_ACTION_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::EVENT_ACTION_LOG_FILENAME).absoluteFilePath());
        _settingsServer.setValue(ServerLog::DEFAULT_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::DEFAULT_LOG_FILENAME).absoluteFilePath());
        _settingsServer.endGroup();

        _settingsServer.beginGroup(tgBotField::TG_BOT_GROUP);
        _settingsServer.setValue(tgBotField::TGBOTTOKEN, "");
        _settingsServer.setValue(tgBotField::TGBOTLOG, QFileInfo({QCoreApplication::applicationDirPath()}, tgBotFileName::TGBOT_LOG_FILENAME).absoluteFilePath());
        _settingsServer.setValue(tgBotField::TGBOTTRUSTCLIENTS, QFileInfo({QCoreApplication::applicationDirPath()}, tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME).absoluteFilePath());
        _settingsServer.endGroup();

    }
    if (!QFileInfo::exists(_settingsHosts.fileName()))
    {
        _settingsHosts.beginGroup("Host");
        _settingsHosts.setValue(hostField::connection_type, connectionType::TCP);
        _settingsHosts.setValue(hostField::address, "127.0.0.1");
        _settingsHosts.setValue(hostField::port, 2021);
        _settingsHosts.setValue(hostField::interval, 1000);
        _settingsHosts.setValue(hostField::logDir,  QFileInfo({QCoreApplication::applicationDirPath()}, _settingsHosts.group()).absoluteFilePath());
        _settingsHosts.endGroup();
        _settingsHosts.sync();
    }
}

void IOTV_Server::slotNewClientConnection()
{
    QTcpSocket* socket = _tcpClient->nextPendingConnection();

    auto client = Maker_iotv::client(_iot_clients, _maxClientCount, socket, this);

    if (client == nullptr)
        return;

//    emit _tg_bot->signalSendMsgForAll("Подключён клиент: " + client->stringRepresentation());

    connect(client.get(), &IOTV_Client::signalClientToServerQueryIdentification, this, &IOTV_Server::slotClientToServerQueryIdentification, Qt::QueuedConnection);
    connect(client.get(), &IOTV_Client::signalClientToServerQueryRead, this, &IOTV_Server::slotClientToServerQueryRead, Qt::QueuedConnection);
    connect(client.get(), &IOTV_Client::signalClientToServerQueryWrite, this, &IOTV_Server::slotClientToServerQueryWrite, Qt::QueuedConnection);
    connect(client.get(), &IOTV_Client::signalClientToServerQueryState, this, &IOTV_Server::slotClientToServerQueryState, Qt::QueuedConnection);
    connect(client.get(), &IOTV_Client::signalClientToServerQueryTech, this, &IOTV_Server::slotClientToServerQueryTech, Qt::QueuedConnection);
    connect(client.get(), &IOTV_Client::signalClientToServerLogData, this, &IOTV_Server::slotClientToServerQueryLogData, Qt::QueuedConnection);


    connect(client.get(), &IOTV_Client::signalDisconnected, this, &IOTV_Server::slotClientDisconnected, Qt::QueuedConnection);
    connect(client.get(), &IOTV_Client::signalMovedToThread, this, &IOTV_Server::slotClearClientObj, Qt::QueuedConnection);

    clientOnlineFile();
}

void IOTV_Server::slotClientDisconnected()
{
    IOTV_Client *client = qobject_cast<IOTV_Client *>(sender());

    if (client == nullptr)
        return;

    QString strOut = "Отключен клиент: " + client->stringRepresentation();
    Log::write(CATEGORY::NET, strOut, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
//    emit _tg_bot->signalSendMsgForAll("Отключен клиент: " + client->stringRepresentation());

    // Закрыть все потоки, если такие имеются для данного клиента
    for (auto &host : _iot_hosts)
    {
        for (int i = 0; i < host->getReadChannelLength(); ++i)
            emit host->signalRemoveStreamRead(i, client->stringRepresentation());

        //        for (int i = 0; i < el.first->getWriteChannelLength(); ++i)
        //            el.first->removeStreamWrite(i, this);

    }

//    connect(client, &IOTV_Client::signalMovedToThread,
//            this, &IOTV_Server::slotClearClientObj, Qt::QueuedConnection);

    emit client->signalMoveToThread(QThread::currentThread());
}

void IOTV_Server::slotNewHostConnection()
{
    auto host = Maker_iotv::host_tcp_in(_iot_hosts, _maxHostCount, _tcpReverseHost->nextPendingConnection(), this);

    if (host == nullptr)
        return;

    connect(host.get(), &IOTV_Host::signalIdentRecived, this, &IOTV_Server::clientHostsUpdate, Qt::QueuedConnection);
    connect(host.get(), &IOTV_Host::signalDevicePingTimeOut, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);
    connect(host.get(), &IOTV_Host::signalDisconnected, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);

    connect(host.get(), &IOTV_Host::signalMovedToThread, this, &IOTV_Server::slotClearHostObj, Qt::QueuedConnection);

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

    Log::write(CATEGORY::ERROR, this->objectName() + ": " + strErr,
               Log::Write_Flag::FILE_STDERR,
               ServerLog::DEFAULT_LOG_FILENAME);

    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    //    socket->abort();
    //    delete socket;
    socket->deleteLater();
}

void IOTV_Server::slotPendingDatagrams()
{
    auto host = Maker_iotv::host_broadcast(_udpBroadcast, _iot_hosts, _maxHostCount, this);

    if (host == nullptr)
        return;

    connect(host.get(), &IOTV_Host::signalIdentRecived, this, &IOTV_Server::clientHostsUpdate, Qt::QueuedConnection);
    connect(host.get(), &IOTV_Host::signalDevicePingTimeOut, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);

    connect(host.get(), &IOTV_Host::signalMovedToThread, this, &IOTV_Server::slotClearHostObj, Qt::QueuedConnection);

    clientHostsUpdate();
}

void IOTV_Server::slotDevicePingTimeout()
{
    IOTV_Host *host = dynamic_cast<IOTV_Host *>(sender());
    if (host == nullptr)
        return;

    emit host->signalMoveToThread(QThread::currentThread());
}

void IOTV_Server::slotClientToServerQueryIdentification()
{
    IOTV_Client *client = dynamic_cast<IOTV_Client *>(sender());
    if (client == nullptr)
        return;

    char outData[BUFSIZ];
    QByteArray result;

    if (_iot_hosts.size() == 0)
    {
        auto size = responseIdentificationData(outData, BUFSIZ, NULL, Identification_FLAGS_NONE);
        result = QByteArray(outData, static_cast<int>(size));
    }

    for (const auto &host : _iot_hosts)
    {
        iotv_obj_t *iot = host->convert();
        auto size = responseIdentificationData(outData, BUFSIZ, iot, Identification_FLAGS_NONE);

        result += QByteArray{outData, static_cast<int>(size)};
        clear_iotv_obj(iot);
    }

    emit client->signalServerToClientQueryIdentification(result);
}

void IOTV_Server::slotClientToServerQueryRead(RAII_Header raii_header)
{
    IOTV_Client *client = dynamic_cast<IOTV_Client *>(sender());
    if (client == nullptr)
        return;

    if (raii_header.header() == nullptr || raii_header.header()->pkg == nullptr)
        return;

    const struct Read_Write *pkg = static_cast<const struct Read_Write *>(raii_header.header()->pkg);
    if (pkg == nullptr)
        return;

    QString compareName(QByteArray{pkg->name, pkg->nameSize});
    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&compareName](const auto &iotv_host)
                           {
                               return iotv_host->getName() == compareName;
                           });

    if (it == _iot_hosts.end())
        return;

    auto host = *it;

    ///!!!
    if (pkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
    {
        emit host->signalAddStreamRead(pkg->channelNumber, client->stringRepresentation());
        //        disconnect(host, &IOTV_Host::signalStreamRead, client, &IOTV_Client::slotStreamRead);
        connect(host.get(), &IOTV_Host::signalStreamRead, client, &IOTV_Client::slotStreamRead, Qt::QueuedConnection);

        return;
    }
    else if (pkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
    {
        emit host->signalRemoveStreamRead(pkg->channelNumber, client->stringRepresentation());
        // связать поток клиента и поток хоста через сигналы - слоты!
        //        disconnect(host, &IOTV_Host::signalStreamRead, client, &IOTV_Client::slotStreamRead);

        return;
    }
    ///!!!

    auto iot = host->convert();
    RAII_iot raii_iot(iot);
    clear_iotv_obj(iot);

    emit client->signalServerToClientQueryRead(raii_header, raii_iot);
}

void IOTV_Server::slotClientToServerQueryWrite(RAII_Header raii_header)
{
    IOTV_Client *client = dynamic_cast<IOTV_Client *>(sender());
    if (client == nullptr)
        return;

    if (raii_header.header() == nullptr || raii_header.header()->pkg == nullptr)
        return;

    const struct Read_Write *pkg = static_cast<const struct Read_Write *>(raii_header.header()->pkg);
    if (pkg == nullptr)
        return;

    QString compareName(QByteArray{pkg->name, pkg->nameSize});
    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&compareName](const auto &iotv_host)
                           {
                               return iotv_host->getName() == compareName;
                           });

    if (it != _iot_hosts.end() && (*it)->state() != State_STATE_OFFLINE)
    {
        // Послать данные на устройство напрямую нельзя - разные потоки
        emit (*it)->signalQueryWrite(pkg->channelNumber, QByteArray(pkg->data, static_cast<qsizetype>(pkg->dataSize)));
    }
}

void IOTV_Server::slotClientToServerQueryState(RAII_Header raii_header)
{
    IOTV_Client *client = dynamic_cast<IOTV_Client *>(sender());
    if (client == nullptr)
        return;

    if (raii_header.header() == nullptr || raii_header.header()->pkg == nullptr)
        return;

    const struct State *pkg = static_cast<const struct State *>(raii_header.header()->pkg);
    if (pkg == nullptr)
        return;

    QString compareName(QByteArray{pkg->name, pkg->nameSize});
    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&compareName](const auto &iotv_host)
                           {
                               return iotv_host->getName() == compareName;
                           });

    if (it == _iot_hosts.end())
        return;

    auto iot = (*it)->convert();
    RAII_iot raii_iot(iot);
    clear_iotv_obj(iot);

    emit client->signalServerToClientQueryState(raii_iot);
}

void IOTV_Server::slotClientToServerQueryTech(RAII_Header raii_header)
{
    IOTV_Client *client = dynamic_cast<IOTV_Client *>(sender());
    if (client == nullptr)
        return;

    if (raii_header.header() == nullptr || raii_header.header()->pkg == nullptr)
        return;

    const struct Tech *pkg = static_cast<const struct Tech *>(raii_header.header()->pkg);
    if (pkg == nullptr)
        return;

    if (pkg->type == Tech_TYPE_EV_AC)
    {
        if (pkg->dataSize == 0)
        {
            //            readEventActionJson();
            //            QByteArray data = Event_Action_Parser::toData(_eventManager->worker());
            emit client->signalFetchEventActionDataFromServer(readEventActionJson());
        }
        else
        {
            QByteArray data(reinterpret_cast<const char *>(pkg->data), pkg->dataSize);

            QJsonParseError err;
            QJsonDocument::fromJson(data, &err);
            if (err.error != QJsonParseError::NoError)
            {
                qDebug() << "Error parse json from client" << err.errorString() << ' ' << err.offset;
                return;
            }

            writeEventActionJson(data);
            readEventActionJson();
        }
    }
}

void IOTV_Server::slotClientToServerQueryLogData(RAII_Header raii_header)
{
    IOTV_Client *client = dynamic_cast<IOTV_Client *>(sender());
    if (client == nullptr)
        return;

    if (raii_header.header() == NULL)
        return;

    struct Log_Data *pkg = static_cast<struct Log_Data *>(raii_header.header()->pkg);
    if (pkg == nullptr || pkg->name == nullptr)
    {
        emit client->signalServerToClientQueryLogData(raii_header, "", true);
        return;
    }
    QString compareName(QByteArray{pkg->name, pkg->nameSize});
    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&compareName](const auto &iotv_host)
                           {
                               return iotv_host->getName() == compareName;
                           });

    if (it == _iot_hosts.end())
    {
        emit client->signalServerToClientQueryLogData(raii_header, "", true);
        return;
    }

    auto host = *it;

    time_t time = pkg->startInterval / 1000;
    std::tm *tm_ptr = localtime(&time);
    QDate startDate(tm_ptr->tm_year + 1900, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);

    QString fileName = host->logName(startDate, pkg->channelNumber);

    emit client->signalServerToClientQueryLogData(raii_header, fileName, false);
}

void IOTV_Server::slotEvent(QString group, QString name, QString type)
{
    QString msg = "\"" + group + "\" \"" + name + "\" " + type;
    Log::write(CATEGORY::EVENT, msg , Log::Write_Flag::FILE_STDOUT, ServerLog::EVENT_ACTION_LOG_FILENAME);

//    emit _tg_bot->signalSendMsgForAll("Событие: " + msg);
}

void IOTV_Server::slotAction(QString group, QString name, QString type)
{
    QString msg = "\"" + group + "\" \"" + name + "\" " + type;
    Log::write(CATEGORY::ACTION, msg , Log::Write_Flag::FILE_STDOUT, ServerLog::EVENT_ACTION_LOG_FILENAME);

//    emit _tg_bot->signalSendMsgForAll("Действие: " + msg);
}

void IOTV_Server::slotBotRequest(int64_t id, QString request)
{
    if (request == "clients")
    {
        QFile file(ServerLog::CLIENT_ONLINE_LOG_FILENAME);
        file.open(QIODevice::ReadOnly);

        QString text = file.readAll();
        if (text.size() == 0)
            text = "Нет подключенных клиентов";

//        emit _tg_bot->signalSendMsg(id, text);
    }
}

void IOTV_Server::slotClearClientObj(QThread *thread)
{
    IOTV_Client *client = dynamic_cast<IOTV_Client *>(sender());

    Q_ASSERT(client);
    Q_ASSERT(thread);

    for (auto &el : _iot_clients)
    {
        if (el.get() == client)
        {
            _iot_clients.erase(el);
            break;
        }
    }
    qDebug() << "delete client";

    thread->exit();
    thread->wait();
    delete thread;
    qDebug() << "delete client thread";

    clientOnlineFile();

    if (_iot_hosts.size() == 0 && _iot_clients.size() == 0)
        emit signalReadyToDestroy();
}

void IOTV_Server::slotClearHostObj(QThread *thread)
{
    IOTV_Host *host = dynamic_cast<IOTV_Host *>(sender());

    Q_ASSERT(host);
    Q_ASSERT(thread);

    for (auto &el : _iot_hosts)
    {
        if (el.get() == host)
        {
            _iot_hosts.erase(el);
            break;
        }
    }
    qDebug() << "delete host";

    thread->exit();
    thread->wait();
    delete thread;
    qDebug() << "delete host thread";

    clientHostsUpdate();

    if (_iot_hosts.size() == 0 && _iot_clients.size() == 0)
        emit signalReadyToDestroy();
}

void IOTV_Server::slotDestroy()
{
    // TCP сервер
    while (_tcpClient->isListening())
    {
        _tcpClient->close();
        QThread::usleep(10);
    }

    Log::write(CATEGORY::NET, "Stop TCP server.", Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);

    if (_iot_clients.size() == 0 && _iot_hosts.size() == 0)
        emit signalReadyToDestroy();

    for(auto &client : _iot_clients)
        emit client->signalMoveToThread(QThread::currentThread());

    for(auto &host : _iot_hosts)
        emit host.get()->signalMoveToThread(QThread::currentThread());

    //    _tgbot_thread->exit();
    //    _tgbot_thread->wait();
}
