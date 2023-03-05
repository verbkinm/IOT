#pragma once

#include <QSettings>
#include <QTcpServer>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QThread>
#include <QCoreApplication>

#include <list>
#include <fstream>
#include <unordered_map>

#include "IOTV_Host/iotv_host.h"
#include "IOTV_Client/iotv_client.h"
#include "log.h"

class IOTV_Server : public QTcpServer
{
    Q_OBJECT
public:
    IOTV_Server(QObject *parent = nullptr);
    ~IOTV_Server();

    QStringList getFileSettingNames() const;

private:
    void checkSettingsFileExist();
    void readServerSettings();
    void readHostSetting();
    void startTCPServer();

    void clientOnlineFile() const;

    std::unordered_map<IOTV_Host* , QThread*> _iot_hosts;
    std::unordered_map<IOTV_Client*, QThread*> _iot_clients;

    QSettings _settingsServer, _settingsHosts;

    QString _address;
    quint16 _port;

    QTimer _reconnectTimer;

    uint _maxClientCount;

private slots:
    void slotNewConnection();
    void slotDisconnected();

    void slotError(QAbstractSocket::SocketError error);
};
