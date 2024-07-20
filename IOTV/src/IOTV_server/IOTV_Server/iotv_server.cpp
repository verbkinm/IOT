#include "iotv_server.h"

#include "event_action_parser.h"
#include "connection_type/tcp_conn_type.h"
#include "maker_iotv.h"
#include "eraser_iotv.h"
#include "tg_bot_handler.h"

#include <QNetworkDatagram>

IOTV_Server::IOTV_Server(QObject *parent) : QObject(parent),
    _settingsServer(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Server"),
    _settingsHosts(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Hosts"),
    _tgbot_thread(nullptr), _tg_bot(nullptr)
{
    _config.checkSettingsFileExist(_settingsServer, _settingsHosts);
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
    qDebug() << "IOTV_Server destruct";

    Eraser_iotv::clients(_iot_clients);
    Eraser_iotv::hosts(_iot_hosts);

    while (_tcpClient->isListening())
    {
        _tcpClient->close();

        QThread::usleep(10);
    }
    Log::write(CATEGORY::NET, "Stop TCP server.", Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);

    Eraser_iotv::bot(&_tg_bot, &_tgbot_thread);
}

QStringList IOTV_Server::getFileSettingNames() const
{
    return {_settingsServer.fileName(), _settingsHosts.fileName()};
}

void IOTV_Server::readServerSettings()
{
    _config.readServerSettings(_settingsServer);

    QString token;
    std::set<int64_t> clients;
    _config.readBotSettings(_settingsServer, token, clients);

    if (token != "")
    {
       Maker_iotv::bot(&_tg_bot, &_tgbot_thread, token, clients);

       if (_tg_bot != nullptr)
       {
           connect(_tg_bot, &IOTV_Bot::signalBotRequest, this, &IOTV_Server::slotBotRequest, Qt::QueuedConnection);
           emit _tg_bot->signalStart();
       }
    }
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

        auto host = Maker_iotv::host(_iot_hosts, _config.maxHostCount, setting, nullptr, this);
        if (host != nullptr)
            connect(host, &IOTV_Host::signalIdentRecived, this, &IOTV_Server::slotHostsUpdate, Qt::QueuedConnection);

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
            Log::write(CATEGORY::ERROR, "Невозможно создать/записать файл: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
            return {};
        }
        file.write("{{}\n}");
        file.close();
        file.open(QIODevice::ReadOnly);

        // Если запись разрешена, а чтение всё же не доступно
        if (!file.isOpen())
        {
            Log::write(CATEGORY::ERROR, "Невозможно прочитать файл: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
            return {};
        }
    }

    QByteArray result = file.readAll();

    auto pairs = Event_Action_Parser::parseJson(result, baseHostList());
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
        Log::write(CATEGORY::ERROR, "Невозможно создать/записать файл: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
        exit(1);
    }

    file.write(data);
}

void IOTV_Server::startTCP(QTcpServer *socket, quint16 port, const QString &lbl)
{
    if (socket->isListening())
        return;

    if (socket->listen(QHostAddress(_config.address), port))
    {
        QString str = "Запуск TCP сервера для " + lbl + " " + _config.address + ":" + QString::number(port);
        Log::write(CATEGORY::NET, str, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
    }
    else
    {
        QString str = "Запуск TCP сервера для " + lbl + " " + _config.address + ":" + QString::number(port);
        Log::write(CATEGORY::ERROR, str, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
    }
}

void IOTV_Server::startUDP(QUdpSocket *socket, const QString &addr, quint16 port, const QString &lbl)
{
    if (socket->bind(QHostAddress(addr), port, QAbstractSocket::ReuseAddressHint))
    {
        QString str = "Запуск UDP сервера для " + lbl + addr + ":" + QString::number(port);
        Log::write(CATEGORY::NET, str, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
    }
    else
    {
        QString str = "Запуск UDP сервера для " + lbl + " " + addr + ":" + QString::number(port);;
        Log::write(CATEGORY::ERROR, str, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
    }
}

void IOTV_Server::startTCPServers()
{
    startTCP(_tcpClient, _config.portForClients, "clients");
    startTCP(_tcpReverseHost, _config.portTcpHosts, "hosts");

    if (_tcpClient->isListening() && _tcpReverseHost->isListening())
        _reconnectTimer.stop();
    else
        _reconnectTimer.start(TCP_conn_type::SERVER_RECONNECT_INTERVAL);
}

void IOTV_Server::startUDPServers()
{
    startUDP(_udpBroadcast, "255.255.255.255", _config.broadcasrListenerPort, "broadcast ");
}

void IOTV_Server::updateClientOnlineFile() const
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

void IOTV_Server::slotHostsUpdate()
{
    IOTV_Host *host = dynamic_cast<IOTV_Host *>(sender());

    if (host != nullptr)
    // Если хост поменял имя и оно не уникально, добавляем к имени номер
    {
        QString name = host->getName();
        int countName = 0;

        for (const auto &el : _iot_hosts)
        {
            if (el.first->getName() == name)
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
            iotv_obj_t *iot = host.first->convert();
            auto size = responseIdentificationData(outData, BUFSIZ, iot, Identification_FLAGS_NONE);

            result += QByteArray{outData, static_cast<int>(size)};
            clear_iotv_obj(iot);
        }
    }

    readEventActionJson();

    for (const auto &client : _iot_clients)
        emit client.first->signalUpdateHosts(result);
}

std::forward_list<const Base_Host *> IOTV_Server::baseHostList() const
{
    std::forward_list<const Base_Host *> result;
    std::transform(_iot_hosts.begin(), _iot_hosts.end(), std::front_inserter(result), [](auto const &pair){
        return pair.first;
    });

    return result;
}

void IOTV_Server::slotNewClientConnection()
{
    QTcpSocket* socket = _tcpClient->nextPendingConnection();

    auto client = Maker_iotv::client(_iot_clients, _config.maxClientCount, socket, this);

    if (client == nullptr)
        return;

    if (_tg_bot != nullptr)
        emit _tg_bot->signalSendMsgForAll("Подключён клиент: " + client->stringRepresentation());

    connect(client, &IOTV_Client::signalClientToServerQueryIdentification, this, &IOTV_Server::slotClientToServerQueryIdentification, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerQueryRead, this, &IOTV_Server::slotClientToServerQueryRead, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerQueryWrite, this, &IOTV_Server::slotClientToServerQueryWrite, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerQueryState, this, &IOTV_Server::slotClientToServerQueryState, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerQueryTech, this, &IOTV_Server::slotClientToServerQueryTech, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerLogData, this, &IOTV_Server::slotClientToServerQueryLogData, Qt::QueuedConnection);

    connect(client, &IOTV_Client::signalDisconnected, this, &IOTV_Server::slotClientDisconnected, Qt::QueuedConnection);

    updateClientOnlineFile();
}

void IOTV_Server::slotClientDisconnected()
{
    IOTV_Client* client = qobject_cast<IOTV_Client*>(sender());

    if (client == nullptr)
        return;

    QString strOut = "Client disconnected";
    Log::write(CATEGORY::NET, strOut, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
    if (_tg_bot != nullptr)
        emit _tg_bot->signalSendMsgForAll("Отключен клиент: " + client->stringRepresentation());

    Eraser_iotv::client(_iot_clients, client);

    updateClientOnlineFile();
}

void IOTV_Server::slotNewHostConnection()
{
    auto host = Maker_iotv::host_tcp_in(_iot_hosts, _config.maxHostCount, _tcpReverseHost->nextPendingConnection(), this);

    if (host == nullptr)
        return;

    connect(host, &IOTV_Host::signalIdentRecived, this, &IOTV_Server::slotHostsUpdate, Qt::QueuedConnection);
    connect(host, &IOTV_Host::signalDevicePingTimeOut, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);
    connect(host, &IOTV_Host::signalDisconnected, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);

    slotHostsUpdate();
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
    auto host = Maker_iotv::host_broadcast(_udpBroadcast, _iot_hosts, _config.maxHostCount, this);

    if (host == nullptr)
        return;

    connect(host, &IOTV_Host::signalIdentRecived, this, &IOTV_Server::slotHostsUpdate, Qt::QueuedConnection);
    connect(host, &IOTV_Host::signalDevicePingTimeOut, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);

    slotHostsUpdate();
}

void IOTV_Server::slotDevicePingTimeout()
{
    IOTV_Host *host = dynamic_cast<IOTV_Host *>(sender());

    Eraser_iotv::host(_iot_hosts, host);

    slotHostsUpdate();
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
        iotv_obj_t *iot = host.first->convert();
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

    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&](const auto &iotv_host)
                           {
                               QString name = iotv_host.first->getName();
                               QString compareName(QByteArray{pkg->name, pkg->nameSize});

                               return name == compareName;
                           });

    if (it == _iot_hosts.end())
        return;

    IOTV_Host *host = it->first;

    ///!!!
    if (pkg->flags == ReadWrite_FLAGS_OPEN_STREAM)
    {
        emit host->signalAddStreamRead(pkg->channelNumber, client->stringRepresentation());
        disconnect(host, &IOTV_Host::signalStreamRead, client, &IOTV_Client::slotStreamRead);
        connect(host, &IOTV_Host::signalStreamRead, client, &IOTV_Client::slotStreamRead, Qt::QueuedConnection);

        return;
    }
    else if (pkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
    {
        emit host->signalRemoveStreamRead(pkg->channelNumber, client->stringRepresentation());
        // связать поток клиента и поток хоста через сигналы - слоты!
        disconnect(host, &IOTV_Host::signalStreamRead, client, &IOTV_Client::slotStreamRead);

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

    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&](const auto &iotv_host)
                           {
                               return iotv_host.first->getName() == QByteArray{pkg->name, pkg->nameSize};
                           });

    if (it != _iot_hosts.end() && it->first->state() != State_STATE_OFFLINE)
    {
        // Послать данные на устройство напрямую нельзя - разные потоки
        emit it->first->signalQueryWrite(pkg->channelNumber, QByteArray(pkg->data, static_cast<qsizetype>(pkg->dataSize)));
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

    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&](const auto &iotv_host)
                           {
                               return iotv_host.first->getName() == QByteArray{pkg->name, pkg->nameSize};
                           });

    if (it == _iot_hosts.end())
        return;

    auto iot = it->first->convert();
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

    QByteArray data(reinterpret_cast<const char *>(pkg->data), pkg->dataSize);

    if (pkg->type == Tech_TYPE_EV_AC)
    {
        if (pkg->dataSize == 0)
            emit client->signalFetchEventActionDataFromServer(readEventActionJson());
        else
        {
            QJsonParseError err;
            QJsonDocument::fromJson(data, &err);

            if (err.error != QJsonParseError::NoError)
            {
                Log::write(CATEGORY::ERROR, "Error parse json from client: " + err.errorString() + " " + QString::number(err.offset),
                           Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
                return;
            }

            writeEventActionJson(data);
            readEventActionJson();
        }
    }
    else if(pkg->type == Tech_TYPE_RUN_EVENT)
    {
        std::pair<QString, QString> groupName_eventName = Event_Action_Parser::parseRunEventAction(data);
        _eventManager->runEvent(groupName_eventName.first, groupName_eventName.second);
    }
    else if(pkg->type == Tech_TYPE_RUN_ACTION)
    {
        std::pair<QString, QString> groupName_actioName = Event_Action_Parser::parseRunEventAction(data);
        _eventManager->runAction(groupName_actioName.first, groupName_actioName.second);
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

    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&](const auto &iotv_host)
                           {
                               return iotv_host.first->getName() == QByteArray{pkg->name, pkg->nameSize};
                           });

    if (it == _iot_hosts.end())
    {
        emit client->signalServerToClientQueryLogData(raii_header, "", true);
        return;
    }

    auto host = it->first;

    time_t time = pkg->startInterval / 1000;
    std::tm *tm_ptr = localtime(&time);
    QDate startDate(tm_ptr->tm_year + 1900, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);

    QString fileName = host->logName(startDate, pkg->channelNumber);

    emit client->signalServerToClientQueryLogData(raii_header, fileName, false);
}

void IOTV_Server::slotEvent(QString group, QString name, QString type)
{
    QString msg = "Event"
                  ": \"" + group + "\" \"" + name + "\" " + type;
    Log::write(CATEGORY::EVENT, msg , Log::Write_Flag::FILE_STDOUT, ServerLog::EVENT_ACTION_LOG_FILENAME);

    if (_tg_bot != nullptr)
        emit _tg_bot->signalSendMsgForAll(msg);
}

void IOTV_Server::slotAction(QString group, QString name, QString type)
{
    QString msg = "Action: \"" + group + "\" \"" + name + "\" " + type;
    Log::write(CATEGORY::ACTION, msg , Log::Write_Flag::FILE_STDOUT, ServerLog::EVENT_ACTION_LOG_FILENAME);

    if (_tg_bot != nullptr)
        emit _tg_bot->signalSendMsgForAll(msg);
}

void IOTV_Server::slotBotRequest(int64_t id, QString request)
{
    if (request == TG_BOT::CMD_CLIENTS)
    {
        updateClientOnlineFile();
        tg_bot_handler::cmd_clients(_tg_bot, id);
    }
    else if (request == TG_BOT::CMD_HOSTS)
        tg_bot_handler::cmd_hosts(_tg_bot, id, _iot_hosts);
    else if (request == TG_BOT::CMD_CONFIG_SERVER)
        tg_bot_handler::cmd_conf_server(_tg_bot, id, _settingsServer.fileName());
    else if (request == TG_BOT::CMD_CONFIG_HOSTS)
        tg_bot_handler::cmd_conf_hosts(_tg_bot, id, _settingsHosts.fileName());
}
