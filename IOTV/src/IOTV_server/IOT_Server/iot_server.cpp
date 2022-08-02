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
    Log::write("Stop TCP server.", Log::Write_Flag::FILE_STDOUT, _logFile);
    while (isListening())
    {
        close();
        QThread::sleep(1);
    }
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

    for (const auto &group : _settingsHosts.childGroups())
    {
        _settingsHosts.beginGroup(group);
        IOT_Host_StructSettings structSettings;

        structSettings.name = group;
        structSettings.connection_type = _settingsHosts.value("connection_type", "TCP").toString();
        structSettings.address = _settingsHosts.value("address", "127.0.0.1").toString();
        structSettings.interval = _settingsHosts.value("interval", 1000).toUInt();
        structSettings.logFile = _settingsHosts.value("log_file", "").toString();

        _iot_hosts.emplace_back(std::make_unique<IOT_Host>(structSettings));

//        QByteArray ba;
//        Base_Host *bh = _iot_hosts.back().get();
//        IOTV_SH::response_WAY(*bh, ba);

        connect(_iot_hosts.back().get(), &IOT_Host::signalResponse_Way, this, &IOT_Server::slotResponse_Way, Qt::QueuedConnection);

        _iot_hosts.back()->runInNewThread();

//        if (connection_type == "TCP")
//        {
//            quint16 port = _settingsHosts.value("port", 2021).toUInt();
//            _iot_hosts.back()->setConnectionTypeTCP(address, port);
//        }
//        else if (connection_type == "COM")
//        {
//            COM_conn_type::SetingsPort settingsPort;
//            settingsPort.baudRate = _settingsHosts.value("baudRate", 115200).toInt();
//            settingsPort.dataBits = _settingsHosts.value("dataBits", 8).toInt();
//            settingsPort.parity = _settingsHosts.value("parity", 0).toInt();
//            settingsPort.stopBits = _settingsHosts.value("stopBits", 1).toInt();
//            settingsPort.flowControl = _settingsHosts.value("flowControl", 0).toInt();

//            _iot_hosts.back()->setConnectionTypeCom(address, settingsPort);
//        }
//        else if (connection_type == "FILE")
//        {
//            _iot_hosts.back()->setConnectionTypeFile(address);
//        }
//        else if (connection_type == "UDP")
//        {
//            _iot_hosts.back()->setConnectionTypeFile(address);
//        }
//        else
//        {
//            Log::write("Error: settings file syntax error, [" + group + "]", Log::Write_Flag::FILE_STDERR);
//            exit(1);
//        }

//        _iot_hosts.back()->setInterval(interval);
//        _iot_hosts.back()->setLogFile(logFile);

        _settingsHosts.endGroup();
//        _iot_hosts.back()->connectToHost();

    }
}

void IOT_Server::startTCPServer()
{
    connect(this, &QTcpServer::newConnection, this, &IOT_Server::slotNewConnection);

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

void IOT_Server::writeToSocket(QTcpSocket *socket, const QByteArray &data)
{
    socket->write(data);
    Log::write("Send to client " + socket->peerAddress().toString() + ":"
               + QString::number(socket->peerPort())
               + " -> " + data.toHex(':'), Log::Write_Flag::FILE_STDOUT, _logFile);
}

void IOT_Server::clinetOnlineFile() const
{
    std::ofstream file("client_online.log", std::ios::trunc);
    if (!file.is_open())
    {
        Log::write("Can't open client_online.log", Log::Write_Flag::STDERR);
        return;
    }

    for(const auto &client : _clientList)
    {
        file << client->peerName().toStdString() << ": "
             << client->peerAddress().toString().toStdString()
             << ":" << std::to_string(client->peerPort()) << "\n";
    }

    file.close();
}

QString IOT_Server::getProgramVersion() const
{
    return _programVersion;
}

void IOT_Server::checkSettingsFileExist()
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

void IOT_Server::slotNewConnection()
{
    QTcpSocket* socket = this->nextPendingConnection();
    if (!socket)
    {
        Log::write("!!! nextPendingConnection: ", Log::Write_Flag::FILE_STDOUT, "New_connection.log"); //debug
        return;
    }

    _clientList.push_back(socket);

    Log::write("Client new connection: " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()),
               Log::Write_Flag::FILE_STDOUT, _logFile);

    connect(socket, &QTcpSocket::readyRead, this, &IOT_Server::slotDataRecived);
    connect(socket, &QTcpSocket::disconnected, this, &IOT_Server::slotDisconnected);
//!!! deleteLater в слоте slotDisconnected
//    connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);

    clinetOnlineFile();
}

//!!! слишком большой метод, должен выполняться для каждого клиента в отдельном потоке!
void IOT_Server::slotDataRecived()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr)
        return;

    _server_buffer_data += socket->readAll();

    while(_server_buffer_data.size())
    {
        auto accumQueryPacket= IOTV_SC::accumQueryPacket(_server_buffer_data);

        if (!accumQueryPacket.first)
        {
            _server_buffer_data.clear();
            return;
        }
        if (accumQueryPacket.first && accumQueryPacket.second)
        {
            QByteArray packetData = _server_buffer_data.mid(0, accumQueryPacket.second);
            Log::write("Client data recived form " + socket->peerAddress().toString() + ":"
                       + QString::number(socket->peerPort())
                       + " <- " + packetData.toHex(':'), Log::Write_Flag::FILE_STDOUT, _logFile);

            QByteArray buff;
            IOTV_SC::Query_Type dataType = IOTV_SC::checkQueryData(packetData);

            if (dataType == IOTV_SC::Query_Type::QUERY_DEVICE_LIST)
            {
                buff.append(RESPONSE_DEVICE_LIST_BYTE);
                buff.append(_iot_hosts.size());

                for(const auto &host : _iot_hosts)
                {
                    QByteArray packetDevice;
                    IOTV_SC::responceToClient_Device_One(*host, packetDevice);
                    buff.append(packetDevice);
                }
                writeToSocket(socket, buff);
            }
            else if (dataType == IOTV_SC::Query_Type::QUERY_STATE)
            {
                QString deviceName;
                if (IOTV_SC::queryName(packetData, deviceName))
                {
                    auto findDevice = std::ranges::find_if (_iot_hosts, [deviceName](std::unique_ptr<IOT_Host> &host){ return host.get()->getName() == deviceName; });
                    if (findDevice != _iot_hosts.end())
                    {
                        IOTV_SC::responceToClient_State(*findDevice->get(), packetData);
                        writeToSocket(socket, packetData);
                    }
                    else
                        Log::write("Client send data to unknow device name - " + deviceName, Log::Write_Flag::FILE_STDOUT, _logFile);
                }
            }
            else if (dataType == IOTV_SC::Query_Type::QUERY_READ)
            {
                QString deviceName;
                if (IOTV_SC::queryName(packetData, deviceName))
                {
                    auto findDevice = std::ranges::find_if (_iot_hosts, [deviceName](std::unique_ptr<IOT_Host> &host){ return host.get()->getName() == deviceName; });
                    if (findDevice != _iot_hosts.end())
                    {
                        IOTV_SC::responceToClient_Read(*findDevice->get(), packetData);
                        writeToSocket(socket, packetData);
                    }
                    else
                        Log::write("Client send data to unknow device name - " + deviceName, Log::Write_Flag::FILE_STDOUT, _logFile);
                }
            }
            else if (dataType == IOTV_SC::Query_Type::QUERY_WRITE)
            {
                QString deviceName;
                if (IOTV_SC::queryName(packetData, deviceName))
                {
                    auto findDevice = std::ranges::find_if (_iot_hosts, [&deviceName](std::unique_ptr<IOT_Host> &host)
                    {
                            return host.get()->getName() == deviceName;
                });

                    if (findDevice != _iot_hosts.end())
                    {
                        uint8_t nameLength = packetData.at(0) >> 3;
                        uint8_t channelNumber = packetData.at(1) & 0x0F;

                        quint16 dataLength = (packetData.at(2) >> 8) | packetData.at(3);

                        QByteArray data = packetData.mid(4 + nameLength, dataLength);
                        Raw::RAW raw{0};

                        if (findDevice->get()->getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR)
                        {
                            raw.str = new char[data.size()];

                            //!!! memcpy
                            for (uint8_t i = 0; i < data.size(); ++i)
                                raw.str[i] = data.at(i);
                        }
                        else
                        {
                            //!!! memcpy
                            for (uint8_t i = 0; i < Raw::size; ++i)
                                raw.array[i] = data.at(i);
                        }

                        QByteArray dataSend = IOTV_SH::query_WRITE(*findDevice->get(), channelNumber, raw);
                        findDevice->get()->writeToServer(dataSend);

                        if (findDevice->get()->getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR)
                            delete[] raw.str;

                        IOTV_SC::responceToClient_Write(packetData);
                        writeToSocket(socket, packetData);
                    }
                    else
                        Log::write("Client send data to unknow device name - " + deviceName, Log::Write_Flag::FILE_STDOUT, _logFile);
                }
            }
            _server_buffer_data = _server_buffer_data.mid(accumQueryPacket.second);
        }
        else
            break;
    }
}

void IOT_Server::slotDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    _clientList.remove(socket);

//!!!
    disconnect(socket, &QTcpSocket::readyRead, this, &IOT_Server::slotDataRecived);
    disconnect(socket,  &QTcpSocket::disconnected, this, &IOT_Server::slotDisconnected);

    QString strOut = "Client disconnected from " + socket->peerAddress().toString()
            + ":" + QString::number(socket->peerPort());
    Log::write(strOut, Log::Write_Flag::FILE_STDOUT, _logFile);
    socket->deleteLater();

    clinetOnlineFile();
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

    Log::write(this->objectName() + ": " + strErr, Log::Write_Flag::FILE_STDERR, _logFile);

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
