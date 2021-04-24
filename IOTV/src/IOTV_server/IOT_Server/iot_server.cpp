#include "iot_server.h"

IOT_Server::IOT_Server(QObject *parent) : QObject(parent),
    _settings(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Server"),
    _programVersion("0.1")
{
    readSettings();
}

QString IOT_Server::getFileSettingName() const
{
    return _settings.fileName();
}

void IOT_Server::readSettings()
{
    for (auto &group : _settings.childGroups())
    {
        _settings.beginGroup(group);

        QString connection_type = _settings.value("connection_type", "ETHERNET").toString();
        QString address = _settings.value("address", "127.0.0.1").toString();
        quint16 port = _settings.value("port", 2021).toUInt();
        uint interval = _settings.value("interval", 0).toUInt();
        QString logFile = _settings.value("log_file", "").toString();

        _iot_hosts.push_back(std::make_unique<IOT_Host>(group));

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

        _settings.endGroup();
        _iot_hosts.back()->connectToHost();
    }
}

QString IOT_Server::getProgramVersion() const
{
    return _programVersion;
}

//void IOT_Server::readData(const QString hostName, uint8_t channelNumber)
//{
//    auto it = std::find_if(_iot_hosts.begin(), _iot_hosts.end(),
//                           [&hostName](const IOT_Host &host){return (hostName == host.getName());});

//    if(it != _iot_hosts.end())
//        (*it)->readData(channelNumber);
//}
