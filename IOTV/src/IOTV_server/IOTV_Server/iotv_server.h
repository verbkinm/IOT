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

    void readEventActionJson();
    void writeEventActionJson(const QByteArray &data);

    void startTCPServers();
    void startTCP(QTcpServer *socket, quint16 port, const QString &lbl);
    void startUDPServers();
    void startUDP(QUdpSocket *socket, const QString &addr, quint16 port, const QString &lbl);

    void clientOnlineFile() const;

    Base_Host *baseHostFromName(const QString &name) const;

    void clientHostsUpdate() const;
    void clientHostsClearAndUpdate();

    // Возвращает список Base_Host* из _iot_hosts
    std::forward_list<const Base_Host *> baseHostList() const;

    std::unordered_map<IOTV_Host* , QThread*> _iot_hosts;
    std::unordered_map<IOTV_Client*, QThread*> _iot_clients;

    QSettings _settingsServer, _settingsHosts;

    QString _address;
    quint16 _portForClients, _portTcpHosts;
    quint16 _broadcasrListenerPort, _portUdpHosts;

    QTimer _reconnectTimer;

    uint _maxClientCount;
    uint _maxHostCount;

    IOTV_Event_Manager *_eventManager;

    QTcpServer *_tcpClient;
    QTcpServer *_tcpReverseHost; // Hosts TCP_REVERSE conn type

    // широковещательный слушатель!
    QUdpSocket *_udpBroadcast;

private slots:
    void slotNewClientConnection();
    void slotClientDisconnected();

    void slotNewHostConnection();

    void slotError(QAbstractSocket::SocketError error);

    void slotFetchEventActionData(QByteArray data);
    void slotQueryEventActionData();

    void slotPendingDatagrams();

    void slotDevicePingTimeout();

    void slotTest();
};
