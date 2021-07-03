#include "iot_server.h"

IOT_Server::IOT_Server(QObject *parent) : QTcpServer(parent),
    _settingsServer(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Server"),
    _settingsHosts(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Hosts"),
    _programVersion("0.1")
{
    checkSettingsFileExist();
    readSettings();
    startTCPServer();

    connect(&_reconnectTimer, &QTimer::timeout, this, &IOT_Server::startTCPServer);
}

IOT_Server::~IOT_Server()
{
    Log::write("Stop TCP server.", Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);
    close();
    while (isListening());  // !!! Возможно зависание программы, если не удастся остановить сервер.
}

QStringList IOT_Server::getFileSettingNames() const
{
    QStringList list;
    list << _settingsServer.fileName() << _settingsHosts.fileName();
    return list;
}

void IOT_Server::readSettings()
{
    _settingsServer.beginGroup("Server");
    _address = _settingsServer.value("address", "127.0.0.1").toString();
    _port = _settingsServer.value("port", 2022).toUInt();
    _logFile = _settingsServer.value("log_file", "server.log").toString();
    _settingsServer.endGroup();

    for (auto &group : _settingsHosts.childGroups())
    {
        _settingsHosts.beginGroup(group);

        QString connection_type = _settingsHosts.value("connection_type", "ETHERNET").toString();
        QString address = _settingsHosts.value("address", "127.0.0.1").toString();
        quint16 port = _settingsHosts.value("port", 2021).toUInt();
        uint interval = _settingsHosts.value("interval", 0).toUInt();
        QString logFile = _settingsHosts.value("log_file", "").toString();

        _iot_hosts.push_back(std::make_shared<IOT_Host>(group));

        if(connection_type == "ETHERNET")
            _iot_hosts.back()->setConnectionTypeEthernet(address, port);
        else if(connection_type == "COM")
            _iot_hosts.back()->setConnectionTypeCom(address);
        else
        {
            Log::write("Error: settings file syntax error, [" + group + "]", Log::Flags::WRITE_TO_FILE_AND_STDERR);
            exit(1);
        }

        _iot_hosts.back()->setInterval(interval);
        _iot_hosts.back()->setLogFile(logFile);

        _settingsHosts.endGroup();
        _iot_hosts.back()->connectToHost();

        connect(_iot_hosts.back().get(), &IOT_Host::signalResponse_Way, this, &IOT_Server::slotResponse_Way);
    }
}

void IOT_Server::startTCPServer()
{
    connect(this, &QTcpServer::newConnection, this, &IOT_Server::slotNewConnection);

    if(!listen(QHostAddress(_address), _port))
    {
        QString str = "Error start TCP server, " + _address + ":" + QString::number(_port);
        Log::write(str, Log::Flags::WRITE_TO_FILE_AND_STDERR, _logFile);
        _reconnectTimer.start(DEFAULT_INTERVAL);
    }
    else
    {
        QString str = "Start TCP server, " + _address + ":" + QString::number(_port);
        Log::write(str, Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);
        _reconnectTimer.stop();
    }
}

void IOT_Server::writeToSocket(QTcpSocket *socket, const QByteArray &data)
{
    socket->write(data);
    Log::write("Send to client " + socket->peerAddress().toString() + ":"
               + QString::number(socket->peerPort())
               + " -> " + data.toHex(':'), Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);
}

QString IOT_Server::getProgramVersion() const
{
    return _programVersion;
}

void IOT_Server::checkSettingsFileExist()
{
    if(!QFileInfo::exists(_settingsServer.fileName()))
    {
        _settingsServer.beginGroup("Server");
        _settingsServer.setValue("address", "127.0.0.1");
        _settingsServer.setValue("port", 2022);
        _settingsServer.setValue("log_file", "server.log");
        _settingsServer.endGroup();
        _settingsServer.sync();
    }
    if(!QFileInfo::exists(_settingsHosts.fileName()))
    {
        _settingsHosts.beginGroup("Default");
        _settingsHosts.setValue("connection_type", "ETHERNET");
        _settingsHosts.setValue("address", "127.0.0.1");
        _settingsHosts.setValue("port", 2021);
        _settingsHosts.setValue("interval", 0);
        _settingsHosts.setValue("log_file", "default.log");
        _settingsHosts.endGroup();
        _settingsHosts.sync();
    }
}

void IOT_Server::slotNewConnection()
{
    QTcpSocket* socket = this->nextPendingConnection();
    if(!socket)
        return;

    _clientList.push_back(socket);

    Log::write("Client new connection: " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()),
               Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);

    connect(socket, &QTcpSocket::readyRead, this, &IOT_Server::slotDataRecived);
    connect(socket, &QTcpSocket::disconnected, this, &IOT_Server::slotDisconnected);
}
// !!!
void IOT_Server::slotDataRecived()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = socket->readAll();

    Log::write("Client data recived form " + socket->peerAddress().toString() + ":"
               + QString::number(socket->peerPort())
               + " <- " + data.toHex(':'), Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);

    for (auto &packetData : IOTV_SC::splitQueryData(data))
    {
        QByteArray buff;
        IOTV_SC::Query_Type dataType = IOTV_SC::checkQueryData(packetData);

        if(dataType == IOTV_SC::Query_Type::QUERY_DEVICE_LIST)
        {
            buff.append(RESPONSE_DEVICE_LIST_BYTE);
            buff.append(_iot_hosts.size());

            for (const auto &host : _iot_hosts)
            {
                QByteArray packetDevice;
                IOTV_SC::responceToClient_Device_One(*host, packetDevice);
                buff.append(packetDevice);
            }
            writeToSocket(socket, buff);
        }
        else if(dataType == IOTV_SC::Query_Type::QUERY_STATE)
        {
            QString deviceName;
            if(IOTV_SC::queryName(packetData, deviceName))
            {
                auto findDevice = std::find_if(_iot_hosts.begin(), _iot_hosts.end(), [deviceName](std::shared_ptr<IOT_Host> &host){ return host.get()->getName() == deviceName; });
                if(findDevice != _iot_hosts.end())
                {
                    IOTV_SC::responceToClient_State(*findDevice->get(), packetData);
                    writeToSocket(socket, packetData);
                }
                else
                    Log::write("Client send data to unknow device name - " + deviceName, Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);
            }
        }
        else if(dataType == IOTV_SC::Query_Type::QUERY_READ)
        {
            QString deviceName;
            if(IOTV_SC::queryName(packetData, deviceName))
            {
                auto findDevice = std::find_if(_iot_hosts.begin(), _iot_hosts.end(), [deviceName](std::shared_ptr<IOT_Host> &host){ return host.get()->getName() == deviceName; });
                if(findDevice != _iot_hosts.end())
                {
                    IOTV_SC::responceToClient_Read(*findDevice->get(), packetData);
                    writeToSocket(socket, packetData);
                }
                else
                    Log::write("Client send data to unknow device name - " + deviceName, Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);
            }
        }
        else if(dataType == IOTV_SC::Query_Type::QUERY_WRITE)
        {
            QString deviceName;
            if(IOTV_SC::queryName(packetData, deviceName))
            {
                auto findDevice = std::find_if(_iot_hosts.begin(), _iot_hosts.end(), [deviceName](std::shared_ptr<IOT_Host> &host){ return host.get()->getName() == deviceName; });
                if(findDevice != _iot_hosts.end())
                {
                    uint8_t nameLength = packetData.at(0) >> 3;
                    uint8_t channelNumber = packetData.at(1) & 0x0F;
//                    QByteArray deviceName = packetData.mid(4, nameLength);

                    uint16_t dataLength = (packetData.at(2) >> 8) | packetData.at(3);

                    QByteArray data = packetData.mid(4 + nameLength, dataLength);
                    Raw::RAW raw;
                    for (uint8_t i = 0; i < Raw::size; ++i)
                        raw.array[i] = data.at(i);

                    IOTV_SH::query_WRITE(*findDevice->get(), channelNumber, raw);
                    IOTV_SC::responceToClient_Write(packetData);
                    writeToSocket(socket, packetData);
                }
                else
                    Log::write("Client send data to unknow device name - " + deviceName, Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);
            }
        }
    }
}

void IOT_Server::slotDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    _clientList.remove(socket);

    disconnect(socket, &QTcpSocket::readyRead, this, &IOT_Server::slotDataRecived);
    disconnect(socket,  &QTcpSocket::disconnected, this, &IOT_Server::slotDisconnected);

    QString strOut = "Client disconnected from " + socket->peerAddress().toString()
            + ":" + QString::number(socket->peerPort());
    Log::write(strOut, Log::Flags::WRITE_TO_FILE_AND_STDOUT, _logFile);
    socket->deleteLater();
}

void IOT_Server::slotError(QAbstractSocket::SocketError error)
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

    Log::write(this->objectName() + ": " + strErr, Log::Flags::WRITE_TO_FILE_AND_STDERR, _logFile);

    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}

void IOT_Server::slotResponse_Way()
{
    IOT_Host *host = qobject_cast<IOT_Host*>(sender());

    QByteArray data;
    IOTV_SC::responceToClient_Device_One(*host, data);

    data.insert(0, RESPONSE_DEVICE_LIST_BYTE);
    data.insert(1, 1);

    for (auto &socket : _clientList)
        writeToSocket(socket, data);
}
