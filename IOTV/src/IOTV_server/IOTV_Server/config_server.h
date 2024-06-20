#pragma once

#include <QSettings>
#include <QString>

#include "set"

class Config_Server
{
public:
    Config_Server();

    QString address;
    quint16 portForClients, portTcpHosts;
    quint16 broadcasrListenerPort, portUdpHosts;

    uint maxClientCount;
    uint maxHostCount;

    void readServerSettings(QSettings &settingObj);
    void readBotSettings(QSettings &settingObj, QString &token, std::set<int64_t> &clients);

    void checkSettingsFileExist(QSettings &server, QSettings &hosts);
};

