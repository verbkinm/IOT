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
    //    qDebug() << QThread::currentThread();

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
    qDebug() << "IOTV_Server destruct";

    for(auto &[client, thread] : _iot_clients)
    {
        client->deleteLater();

        thread->exit();
        thread->wait();
        delete thread;
    }

    _iot_clients.clear();

    for(auto &[host, thread] : _iot_hosts)
    {
        host->deleteLater();

        thread->exit();
        thread->wait();
        delete thread;
    }

    _iot_hosts.clear();


    while (_tcpClient->isListening())
    {
        _tcpClient->close();

        QThread::usleep(10);
    }
    Log::write("Stop TCP server.", Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
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
            connect(host, &IOTV_Host::signalIdentRecived, this, &IOTV_Server::clientHostsUpdate, Qt::QueuedConnection);

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
            Log::write("Can't create/open file: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
            return {};
        }
        file.write("{\"Остальное\" : {}\n}");
        file.close();
        file.open(QIODevice::ReadOnly);

        // Если запись разрешена, а чтение всё же не доступно
        if (!file.isOpen())
        {
            Log::write("Can't create/open file: " + fileName, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
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

    return result;
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
        Log::write(str, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
    }
    else
    {
        QString str = "Error start TCP server for " + lbl + " " + _address + ":" + QString::number(port);
        Log::write(str, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
    }
}

void IOTV_Server::startUDP(QUdpSocket *socket, const QString &addr, quint16 port, const QString &lbl)
{
    if (socket->bind(QHostAddress(addr), port, QAbstractSocket::ReuseAddressHint))
    {
        QString str = "Start UDP server for " + lbl + addr + ":" + QString::number(port);
        Log::write(str, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);
    }
    else
    {
        QString str = "Error start UDP server for " + lbl + " " + addr + ":" + QString::number(port);;
        Log::write(str, Log::Write_Flag::FILE_STDERR, ServerLog::DEFAULT_LOG_FILENAME);
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

//!!! не используется
Base_Host *IOTV_Server::baseHostFromName(const QString &name) const
{
    auto it = std::find_if (_iot_hosts.begin(), _iot_hosts.end(), [&name](const auto &pair){
        return pair.first->getName() == name;
    });

    if (it != _iot_hosts.end())
        return it->first;

    return nullptr;
}

void IOTV_Server::clientHostsUpdate()
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
        _settingsServer.setValue(ServerLog::CLIENT_ONLINE_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::CLIENT_ONLINE_LOG_FILENAME).absoluteFilePath());
        _settingsServer.setValue(ServerLog::DEFAULT_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::DEFAULT_LOG_FILENAME).absoluteFilePath());
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

    connect(client, &IOTV_Client::signalClientToServerQueryIdentification, this, &IOTV_Server::slotClientToServerQueryIdentification, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerQueryRead, this, &IOTV_Server::slotClientToServerQueryRead, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerQueryWrite, this, &IOTV_Server::slotClientToServerQueryWrite, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerQueryState, this, &IOTV_Server::slotClientToServerQueryState, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerQueryTech, this, &IOTV_Server::slotClientToServerQueryTech, Qt::QueuedConnection);
    connect(client, &IOTV_Client::signalClientToServerLogData, this, &IOTV_Server::slotClientToServerQueryLogData, Qt::QueuedConnection);


    connect(client, &IOTV_Client::signalDisconnected, this, &IOTV_Server::slotClientDisconnected, Qt::QueuedConnection);

    clientOnlineFile();
}

void IOTV_Server::slotClientDisconnected()
{
    IOTV_Client* client = qobject_cast<IOTV_Client*>(sender());

    if (client == nullptr)
        return;

    QString strOut = "Client disconnected";
    Log::write(strOut, Log::Write_Flag::FILE_STDOUT, ServerLog::DEFAULT_LOG_FILENAME);

    client->deleteLater();

    _iot_clients[client]->exit();
    _iot_clients[client]->wait();
    delete _iot_clients[client];

    _iot_clients.erase(client);

//    readEventActionJson();

    clientOnlineFile();
}

void IOTV_Server::slotNewHostConnection()
{
    auto host = Maker_iotv::host_tcp_in(_iot_hosts, _maxHostCount, _tcpReverseHost->nextPendingConnection(), this);

    if (host == nullptr)
        return;

    connect(host, &IOTV_Host::signalIdentRecived, this, &IOTV_Server::clientHostsUpdate, Qt::QueuedConnection);
    connect(host, &IOTV_Host::signalDevicePingTimeOut, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);
    connect(host, &IOTV_Host::signalDisconnected, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);

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

    connect(host, &IOTV_Host::signalIdentRecived, this, &IOTV_Server::clientHostsUpdate, Qt::QueuedConnection);
    connect(host, &IOTV_Host::signalDevicePingTimeOut, this, &IOTV_Server::slotDevicePingTimeout, Qt::QueuedConnection);

    clientHostsUpdate();
}

void IOTV_Server::slotDevicePingTimeout()
{
    IOTV_Host *host = dynamic_cast<IOTV_Host *>(sender());
    if (host == nullptr)
        return;

    _iot_hosts[host]->exit();
    connect(_iot_hosts[host], &QThread::finished, this, [this](){
        qDebug() << "delete thread";
        QThread *thread = dynamic_cast<QThread *>(sender());
        delete thread;
    });

    _iot_hosts.erase(host);
    host->deleteLater();

    clientHostsUpdate();
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
        emit host->signalAddStreamRead(pkg->channelNumber, client);
        disconnect(host, &IOTV_Host::signalStreamRead, client, &IOTV_Client::slotStreamRead);
        connect(host, &IOTV_Host::signalStreamRead, client, &IOTV_Client::slotStreamRead, Qt::QueuedConnection);

        return;
    }
    else if (pkg->flags == ReadWrite_FLAGS_CLOSE_STREAM)
    {
        emit host->signalRemoveStreamRead(pkg->channelNumber, client);
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
                qDebug() << "Error parse json " << err.errorString() << ' ' << err.offset;
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

void IOTV_Server::slotTest()
{
    // TESTING
}
