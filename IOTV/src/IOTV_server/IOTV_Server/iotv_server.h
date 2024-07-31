#pragma once

#include <QSettings>
#include <QTcpServer>
#include <QUdpSocket>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QThread>
#include <QCoreApplication>

#include <list>
#include <fstream>
#include <unordered_map>
#include <algorithm>

#include "IOTV_Host/iotv_host.h"
#include "IOTV_Client/iotv_client.h"
#include "IOTV_Bot/iotv_bot.h"

#include "config_server.h"
#include "iotv_event_manager.h"

class IOTV_Server : public QObject
{
    Q_OBJECT
public:
    explicit IOTV_Server(QObject *parent = nullptr);
    ~IOTV_Server();

    QStringList getFileSettingNames() const;

private:
    void readServerSettings();
    void readHostSetting();

    QByteArray readEventActionJson();
    void writeEventActionJson(const QByteArray &data);

    void startTCPServers();
    void startTCP(QTcpServer *socket, quint16 port, const QString &lbl);
    void startUDPServers();
    void startUDP(QUdpSocket *socket, const QString &addr, quint16 port, const QString &lbl);

    void updateClientOnlineFile() const;

    // Возвращает список Base_Host* из _iot_hosts
    std::forward_list<const Base_Host *> baseHostList() const;

    std::unordered_map<IOTV_Host *, QThread *> _iot_hosts;
    std::unordered_map<IOTV_Client *, QThread *> _iot_clients;

    QSettings _settingsServer, _settingsHosts;
    Config_Server _config;

    QTimer _reconnectTimer, _hosts_debug_timer;

    std::shared_ptr<IOTV_Event_Manager> _eventManager;

    QTcpServer *_tcpClient;
    QTcpServer *_tcpReverseHost; // Hosts TCP_REVERSE conn type

    // широковещательный слушатель!
    QUdpSocket *_udpBroadcast;

    IOTV_Bot *_tg_bot;
    QThread *_tgbot_thread;

private slots:
    void slotHostsUpdate();

    void slotNewClientConnection();
    void slotClientDisconnected();

    void slotNewHostConnection();

    void slotError(QAbstractSocket::SocketError error);

    void slotPendingDatagrams();

    void slotDevicePingTimeout();

    void slotClientToServerQueryIdentification();
    void slotClientToServerQueryRead(RAII_Header raii_header);
    void slotClientToServerQueryWrite(RAII_Header raii_header);
    void slotClientToServerQueryState(RAII_Header raii_header);
    void slotClientToServerQueryTech(RAII_Header raii_header);
    void slotClientToServerQueryLogData(RAII_Header raii_header);

    void slotEvent(QString group, QString name, QString type);
    void slotAction(QString group, QString name, QString type);

    void slotBotRequest(int64_t id, QString request);

    void slotHostsDebug();
};
