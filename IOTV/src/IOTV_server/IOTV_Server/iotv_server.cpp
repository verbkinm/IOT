#include "iotv_server.h"

IOTV_Server::IOTV_Server(QObject *parent) : QTcpServer(parent),
    _settingsServer(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Server"),
    _settingsHosts(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Hosts"),
    _eventManager(nullptr)
{
    checkSettingsFileExist();

    readServerSettings();
    startTCPServer();

    readHostSetting();
    readEventActionJson();

    connect(&_reconnectTimer, &QTimer::timeout, this, &IOTV_Server::startTCPServer);
}

IOTV_Server::~IOTV_Server()
{
    for(auto &[_, thread] : _iot_clients)
    {
        thread->exit();
        thread->wait();
        thread->deleteLater();
    }

    _iot_clients.clear();

    for(auto &[_, thread] : _iot_hosts)
    {
        thread->exit();
        thread->wait();
        thread->deleteLater();
    }

    _iot_hosts.clear();


    while (isListening())
    {
        close();

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
    _address = _settingsServer.value("address", "127.0.0.1").toString();
    _port = _settingsServer.value("port", 2022).toUInt();
    _maxClientCount = _settingsServer.value("max_client", 5).toUInt();
    ServerLog::TCP_LOG_FILENAME = _settingsServer.value(ServerLog::TCP_LOG, ServerLog::TCP_LOG_FILENAME).toString();
    ServerLog::CLIENT_ONLINE_LOG_FILENAME = _settingsServer.value(ServerLog::CLIENT_ONLINE_LOG, ServerLog::CLIENT_ONLINE_LOG_FILENAME).toString();
    ServerLog::DEFAULT_LOG_FILENAME = _settingsServer.value(ServerLog::DEFAULT_LOG, ServerLog::DEFAULT_LOG_FILENAME).toString();
    _settingsServer.endGroup();
}

void IOTV_Server::readHostSetting()
{
    for (const auto &group : _settingsHosts.childGroups())
    {
        _settingsHosts.beginGroup(group);
        std::unordered_map<QString, QString> setting;

        setting[hostField::name] = group.toLatin1();
        setting[hostField::name] = setting[hostField::name].toLocal8Bit().size() > 30 ? setting[hostField::name].toLocal8Bit().mid(0, 30) : setting[hostField::name];
        setting[hostField::connection_type] = _settingsHosts.value(hostField::connection_type, "TCP").toString();
        setting[hostField::address] = _settingsHosts.value(hostField::address, "127.0.0.1").toString();
        setting[hostField::interval] = _settingsHosts.value(hostField::interval, "1000").toString();
        setting[hostField::logFile] = _settingsHosts.value(hostField::logFile, setting[hostField::name] + ".log").toString();

        if (setting[hostField::connection_type] == connectionType::TCP || setting[hostField::connection_type] == connectionType::UDP)
            setting[hostField::port] = _settingsHosts.value(hostField::port, "0").toString();

        //!!!
        //if (setting[hostField::connection_type] == connectionType::COM)
        //  ;

        auto it = std::ranges::find_if(_iot_hosts, [&setting](const auto &pair){
            return pair.first->settingsData().at(hostField::name) == setting[hostField::name];
        });

        if (it != _iot_hosts.end())
        {
            Log::write(QString(Q_FUNC_INFO) +
                       ", Error: Double host name in config file - " + setting[hostField::name],
                    Log::Write_Flag::FILE_STDERR,
                    ServerLog::DEFAULT_LOG_FILENAME);
            exit(1);
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
            exit(1);
        }

        //        connect (host, &Base_Host::signalIdentRecived, this, &IOTV_Server::slotTest);

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
        file.close();
        file.open(QIODevice::ReadOnly);
    }

    std::forward_list<const Base_Host *> hosts;
    for(const auto &pair : _iot_hosts)
        hosts.push_front(pair.first);


    auto list = Event_Action_Parser::parseJson(file.readAll(), hosts);

    if (_eventManager != nullptr)
        delete _eventManager;

    _eventManager = new IOTV_Event_Manager(this);

    for(const auto &[event, action] : list)
        _eventManager->bind(event, action);
}

void IOTV_Server::startTCPServer()
{
    connect(this, &QTcpServer::newConnection, this, &IOTV_Server::slotNewConnection);

    if (!listen(QHostAddress(_address), _port))
    {
        QString str = "Error start TCP server, " + _address + ":" + QString::number(_port);
        Log::write(str, Log::Write_Flag::FILE_STDERR, ServerLog::TCP_LOG_FILENAME);
        _reconnectTimer.start(TCP_conn_type::SERVER_RECONNECT_INTERVAL);
    }
    else
    {
        _reconnectTimer.stop();
        QString str = "Start TCP server, " + _address + ":" + QString::number(_port);
        Log::write(str, Log::Write_Flag::FILE_STDOUT, ServerLog::TCP_LOG_FILENAME);
    }
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
    auto it = std::ranges::find_if(_iot_hosts, [&name](const auto &pair){
        return pair.first->getName() == name;
    });

    if (it != _iot_hosts.end())
        return it->first;

    return nullptr;
}

//void IOTV_Server::parseJson(const QByteArray &data)
//{
//    if (_eventManager != nullptr)
//        delete _eventManager;

//    _eventManager = new IOTV_Event_Manager(this);


//}

//IOTV_Event *IOTV_Server::parseEvent(const QJsonObject &jobj) const
//{
//    QString type = jobj.value(Json_Event_Action::TYPE).toString();
//    Base_Host *host = baseHostFromName(jobj.value(Json_Event_Action::HOST_NAME).toString());
//    IOTV_Event *event = nullptr;

//    if (host == nullptr)
//        return event;

//    if (type == Json_Event_Action::TYPE_CONN || type == Json_Event_Action::TYPE_DISCONN)
//    {
//        event = _eventManager->createEvent(host, type);
//    }
//    else if (type == Json_Event_Action::TYPE_STATE)
//    {
//        QString state = jobj.value(Json_Event_Action::TYPE_STATE).toString();
//        event = _eventManager->createEvent(host, type, state);
//    }
//    else if (type == Json_Event_Action::TYPE_DATA)
//    {
//        QString direction = jobj.value(Json_Event_Action::DIRECTION).toString();
//        QString compare = jobj.value(Json_Event_Action::COMPARE).toString();
//        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toInt();
//        QByteArray data = jobj.value(Json_Event_Action::DATA).toVariant().toByteArray();
//        QString dataType = jobj.value(Json_Event_Action::DATA_TYPE).toString();
//        event = _eventManager->createEvent(host, type, direction, data, dataType, compare, ch_num);
//    }

//    return event;
//}

//IOTV_Action *IOTV_Server::parseAction(const QJsonObject &jobj) const
//{
//    QString type = jobj.value(Json_Event_Action::TYPE).toString();
//    IOTV_Action *action = nullptr;

//    if (type == Json_Event_Action::TYPE_DATA_TX)
//    {
//        Base_Host *host = baseHostFromName(jobj.value(Json_Event_Action::HOST_NAME).toString());
//        QByteArray data = jobj.value(Json_Event_Action::DATA).toVariant().toByteArray();
//        QString dataType = jobj.value(Json_Event_Action::DATA_TYPE).toVariant().toString();
//        uint8_t ch_num = jobj.value(Json_Event_Action::CH_NUM).toInt();

//        action = _eventManager->createAction(type, host, ch_num, data, dataType);

//    }
//    else if (type == Json_Event_Action::TYPE_DATA_TX_REF)
//    {
//        Base_Host *dstHost = baseHostFromName(jobj.value(Json_Event_Action::HOST_DST).toString());
//        Base_Host *srctHost = baseHostFromName(jobj.value(Json_Event_Action::HOST_SRC).toString());

//        uint8_t dstChNum = jobj.value(Json_Event_Action::CH_NUM_DST).toInt();
//        uint8_t srcChNum = jobj.value(Json_Event_Action::CH_NUM_SRC).toInt();

//        action = _eventManager->createAction(type, dstHost, srctHost, dstChNum, srcChNum);
//    }

//    return action;
//}

//QByteArray IOTV_Server::toData() const
//{
//    //    _eventManager.createAction();
//}

void IOTV_Server::checkSettingsFileExist()
{
    if (!QFileInfo::exists(_settingsServer.fileName()))
    {
        _settingsServer.beginGroup("Server");
        _settingsServer.setValue(serverField::address, "127.0.0.1");
        _settingsServer.setValue(serverField::port, 2022);
        _settingsServer.setValue(serverField::maxClient , _maxClientCount);
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

void IOTV_Server::slotNewConnection()
{
    QTcpSocket* socket = this->nextPendingConnection();

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
        exit(1);
    }

    connect(client, &IOTV_Client::signalDisconnected, this, &IOTV_Server::slotDisconnected, Qt::QueuedConnection);
    clientOnlineFile();
}

void IOTV_Server::slotDisconnected()
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

void IOTV_Server::slotTest()
{
    // TESTING
}
