#include "iotv_server.h"
#include "IOTV_Client/iotv_client.h"

IOTV_Server::IOTV_Server(QObject *parent) : QTcpServer(parent),
    _settingsServer(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Server"),
    _settingsHosts(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Hosts"),
    _programVersion("0.2")
{
    checkSettingsFileExist();
    readSettings();
    startTCPServer();

    connect(&_reconnectTimer, &QTimer::timeout, this, &IOTV_Server::startTCPServer);
}

IOTV_Server::~IOTV_Server()
{
    Log::write("Stop TCP server.", Log::Write_Flag::FILE_STDOUT, _logFile);
    while (isListening())
    {
        close();
        QThread::sleep(1);
    }
}

QStringList IOTV_Server::getFileSettingNames() const
{
    QStringList list;
    list << _settingsServer.fileName() << _settingsHosts.fileName();
    return list;
}

void IOTV_Server::readSettings()
{       
    _settingsServer.beginGroup("Server");
    _address = _settingsServer.value("address", "127.0.0.1").toString();
    _port = _settingsServer.value("port", 2022).toUInt();
    _logFile = _settingsServer.value("log_file", "server.log").toString();
    _settingsServer.endGroup();

    for (const auto &group : _settingsHosts.childGroups())
    {
        _settingsHosts.beginGroup(group);
        std::unordered_map<QString, QString> setting;

        setting[hostField::name] = group;
        setting[hostField::connection_type] = _settingsHosts.value(hostField::connection_type, "TCP").toString();
        setting[hostField::address] = _settingsHosts.value(hostField::address, "127.0.0.1").toString();
        setting[hostField::interval] = _settingsHosts.value(hostField::interval, "1000").toString();
        setting[hostField::logFile] = _settingsHosts.value(hostField::logFile, "").toString();

        if (setting[hostField::connection_type] == connectionType::TCP)
            setting[hostField::port] = _settingsHosts.value(hostField::port, "0").toString();

        _iot_hosts.emplace_back(setting);

        if (!_iot_hosts.back().runInNewThread())
        {
            Log::write(QString(Q_FUNC_INFO) + " Error: Can't run IOT_Host in new thread", Log::Write_Flag::FILE_STDOUT);
            exit(1);
        }

        _settingsHosts.endGroup();
    }
}

void IOTV_Server::startTCPServer()
{
    connect(this, &QTcpServer::newConnection, this, &IOTV_Server::slotNewConnection);

    if (!listen(QHostAddress(_address), _port))
    {
        QString str = "Error start TCP server, " + _address + ":" + QString::number(_port);
        Log::write(str, Log::Write_Flag::FILE_STDERR, _logFile);
        _reconnectTimer.start(Base_conn_type::DEFAULT_INTERVAL);
    }
    else
    {
        _reconnectTimer.stop();
        QString str = "Start TCP server, " + _address + ":" + QString::number(_port);
        Log::write(str, Log::Write_Flag::FILE_STDOUT, _logFile);
    }
}

void IOTV_Server::clientOnlineFile() const
{
    std::ofstream file("client_online.log", std::ios::trunc);
    if (!file.is_open())
    {
        Log::write("Can't open client_online.log", Log::Write_Flag::FILE_STDERR);
        return;
    }

    for(const auto &client : _iot_clients)
    {
        const QTcpSocket *socket = client.socket();
        file << socket->peerName().toStdString()
             << ": "
             << socket->peerAddress().toString().toStdString()
             << ":"
             << std::to_string(socket->peerPort())
             << "\n";
    }

    file.close();
}

QString IOTV_Server::getProgramVersion() const
{
    return _programVersion;
}

void IOTV_Server::checkSettingsFileExist()
{
    if (!QFileInfo::exists(_settingsServer.fileName()))
    {
        _settingsServer.beginGroup("Server");
        _settingsServer.setValue("address", "127.0.0.1");
        _settingsServer.setValue("port", 2022);
        _settingsServer.setValue("log_file", "server.log");
        _settingsServer.endGroup();
        _settingsServer.sync();
    }
    if (!QFileInfo::exists(_settingsHosts.fileName()))
    {
        _settingsHosts.beginGroup("Default");
        _settingsHosts.setValue("connection_type", "TCP");
        _settingsHosts.setValue("address", "127.0.0.1");
        _settingsHosts.setValue("port", 2021);
        _settingsHosts.setValue("interval", 0);
        _settingsHosts.setValue("log_file", "default.log");
        _settingsHosts.endGroup();
        _settingsHosts.sync();
    }
}

void IOTV_Server::slotNewConnection()
{
    QTcpSocket* socket = this->nextPendingConnection();

    if (!socket)
    {
        Log::write(QString(Q_FUNC_INFO) + " nextPendingConnection: ", Log::Write_Flag::FILE_STDOUT, "New_connection.log");
        exit(1);
    }

    _iot_clients.emplace_back(socket, _iot_hosts);
    if (!_iot_clients.back().runInNewThread())
    {
        Log::write(QString(Q_FUNC_INFO) + " Error: Can't run IOT_Client in new thread ", Log::Write_Flag::FILE_STDOUT);
        exit(1);
    }

    Log::write("Client new connection: "
               + socket->peerAddress().toString()
               + ":"
               + QString::number(socket->peerPort()),
               Log::Write_Flag::FILE_STDOUT, _logFile);

    connect(&_iot_clients.back(), &IOTV_Client::signalDisconnected, this, &IOTV_Server::slotDisconnected);
    //    clientOnlineFile();
}

void IOTV_Server::slotDisconnected()
{
    IOTV_Client* client = qobject_cast<IOTV_Client*>(sender());

    QString strOut = "Client disconnected from "
            + client->socket()->peerAddress().toString()
            + ":" + QString::number(client->socket()->peerPort());

    Log::write(strOut, Log::Write_Flag::FILE_STDOUT, _logFile);

    _iot_clients.remove(*client);
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

    Log::write(this->objectName() + ": " + strErr, Log::Write_Flag::FILE_STDERR, _logFile);

    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}
