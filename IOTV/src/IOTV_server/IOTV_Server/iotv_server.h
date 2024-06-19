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

#include "iotv_event_manager.h"

class IOTV_Server : public QObject
{
    Q_OBJECT
public:
    explicit IOTV_Server(QObject *parent = nullptr);
    ~IOTV_Server();

    QStringList getFileSettingNames() const;

private:
    void checkSettingsFileExist();
    void readServerSettings();
    void readHostSetting();

    QByteArray readEventActionJson();
    void writeEventActionJson(const QByteArray &data);

    void startTCPServers();
    void startTCP(std::shared_ptr<QTcpServer> socket, quint16 port, const QString &lbl);
    void startUDPServers();
    void startUDP(std::shared_ptr<QUdpSocket> socket, const QString &addr, quint16 port, const QString &lbl);

    void clientOnlineFile() const;

    void clientHostsUpdate();

    IOTV_Host_List _iot_hosts;
    IOTV_Client_List _iot_clients;

    QSettings _settingsServer, _settingsHosts;

    QString _address;
    quint16 _portForClients, _portTcpHosts;
    quint16 _broadcasrListenerPort, _portUdpHosts;

    QTimer _reconnectTimer;

    uint _maxClientCount;
    uint _maxHostCount;

    std::shared_ptr<IOTV_Event_Manager> _eventManager;

    std::shared_ptr<QTcpServer> _tcpClient;
    std::shared_ptr<QTcpServer> _tcpReverseHost; // Hosts TCP_REVERSE conn type

    // широковещательный слушатель!
    std::shared_ptr<QUdpSocket> _udpBroadcast;

    std::shared_ptr<IOTV_Bot> _tg_bot;
    std::shared_ptr<QThread> _tgbot_thread;

private slots:
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

    void slotClearClientObj(QThread *thread);
    void slotClearHostObj(QThread *thread);

    void slotDestroy();

signals:
    void signalReadyToDestroy();
    void signalDestroy();
};
