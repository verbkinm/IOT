#pragma once

#include <QSettings>
#include <QTcpServer>
#include <QFileInfo>
#include <QTimer>
#include <QThread>

#include <list>
#include <fstream>

#include "IOTV_Host/iotv_host.h"
#include "log.h"

class IOTV_Client;

class IOTV_Server : public QTcpServer
{
    Q_OBJECT
public:
    IOTV_Server(QObject *parent = nullptr);
    ~IOTV_Server();

    QStringList getFileSettingNames() const;

private:
    void checkSettingsFileExist();
    void readSettings();
    void startTCPServer();

    void clientOnlineFile() const;

    //!!! unordered_set
    std::list<IOTV_Host> _iot_hosts;
    std::list<IOTV_Client> _iot_clients;

    QSettings _settingsServer, _settingsHosts;

    const QString _programVersion;
    QString _address;
    quint16 _port;

    QTimer _reconnectTimer;

    QByteArray _server_buffer_data;

    static constexpr uint _maxClientCount = 5;

private slots:
    void slotNewConnection();
    void slotDisconnected();

    void slotError(QAbstractSocket::SocketError error);
};
