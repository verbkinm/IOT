#include "config_server.h"
#include "ConfigTypes.h"

#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

#include "log.h"

#include <fstream>

Config_Server::Config_Server()
{
    address = "";

    portForClients = 0;
    portTcpHosts = 0;
    broadcasrListenerPort = 0;
    portUdpHosts = 0;

    maxClientCount = 0;
    maxHostCount = 0;
}

void Config_Server::readServerSettings(QSettings &settingObj)
{
    settingObj.beginGroup(serverField::SERVER_GROUP);

    address = settingObj.value(serverField::address, "127.0.0.1").toString();
    portForClients = settingObj.value(serverField::portClients, 2022).toUInt();
    portTcpHosts = settingObj.value(serverField::portHosts, 2023).toUInt();
    broadcasrListenerPort = settingObj.value(serverField::broadCastListenerPort, 2022).toUInt();
    maxClientCount = settingObj.value(serverField::maxClient, 5).toUInt();
    maxHostCount = settingObj.value(serverField::maxHost, 10).toUInt();

    ServerLog::CLIENT_ONLINE_LOG_FILENAME = settingObj.value(ServerLog::CLIENT_ONLINE_LOG, ServerLog::CLIENT_ONLINE_LOG_FILENAME).toString();
    ServerLog::EVENT_ACTION_LOG_FILENAME = settingObj.value(ServerLog::EVENT_ACTION_LOG, ServerLog::EVENT_ACTION_LOG_FILENAME).toString();
    ServerLog::DEFAULT_LOG_FILENAME = settingObj.value(ServerLog::DEFAULT_LOG, ServerLog::DEFAULT_LOG_FILENAME).toString();

    settingObj.endGroup();

    settingObj.sync();
}

void Config_Server::readBotSettings(QSettings &settingObj, QString &token, std::set<int64_t> &clients)
{
    settingObj.beginGroup(tgBotField::TG_BOT_GROUP);
    token = settingObj.value(tgBotField::TGBOTTOKEN, "").toString();

    tgBotFileName::TGBOT_LOG_FILENAME = settingObj.value(tgBotField::TGBOTLOG, tgBotFileName::TGBOT_LOG_FILENAME).toString();
    tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME = settingObj.value(tgBotField::TGBOTTRUSTCLIENTS, tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME).toString();

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
    settingObj.endGroup();
    settingObj.sync();
}

void Config_Server::checkSettingsFileExist(QSettings &server, QSettings &hosts)
{
    if (!QFileInfo::exists(server.fileName()))
    {
        server.beginGroup(serverField::SERVER_GROUP);
        server.setValue(serverField::address, "127.0.0.1");
        server.setValue(serverField::portClients, 2022);
        server.setValue(serverField::portHosts, 2023);
        server.setValue(serverField::broadCastListenerPort, 2022);
        server.setValue(serverField::maxClient, 1);
        server.setValue(serverField::maxHost, 10);
        server.setValue(ServerLog::CLIENT_ONLINE_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::CLIENT_ONLINE_LOG_FILENAME).absoluteFilePath());
        server.setValue(ServerLog::EVENT_ACTION_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::EVENT_ACTION_LOG_FILENAME).absoluteFilePath());
        server.setValue(ServerLog::DEFAULT_LOG, QFileInfo({QCoreApplication::applicationDirPath()}, ServerLog::DEFAULT_LOG_FILENAME).absoluteFilePath());
        server.endGroup();

        server.beginGroup(tgBotField::TG_BOT_GROUP);
        server.setValue(tgBotField::TGBOTTOKEN, "");
        server.setValue(tgBotField::TGBOTLOG, QFileInfo({QCoreApplication::applicationDirPath()}, tgBotFileName::TGBOT_LOG_FILENAME).absoluteFilePath());
        server.setValue(tgBotField::TGBOTTRUSTCLIENTS, QFileInfo({QCoreApplication::applicationDirPath()}, tgBotFileName::TGBOT_TRUST_CLIENTS_FILENAME).absoluteFilePath());
        server.endGroup();
        server.sync();
    }

    if (!QFileInfo::exists(hosts.fileName()))
    {
        hosts.beginGroup("Host");
        hosts.setValue(hostField::connection_type, connectionType::TCP);
        hosts.setValue(hostField::address, "127.0.0.1");
        hosts.setValue(hostField::port, 2021);
        hosts.setValue(hostField::interval, 1000);
        hosts.setValue(hostField::logDir,  QFileInfo({QCoreApplication::applicationDirPath()}, hosts.group()).absoluteFilePath());
        hosts.endGroup();
        hosts.sync();
    }
}
