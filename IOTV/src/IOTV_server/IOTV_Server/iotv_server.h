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

class IOTV_Server : public QTcpServer
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

    void startTCPServer();
    void startBroadCastListener();

    void clientOnlineFile() const;

    Base_Host *baseHostFromName(const QString &name) const;

    void clientHostsUpdate() const;

    //! Возвращает список Base_Host* из _iot_hosts
    std::forward_list<const Base_Host *> baseHostList() const;

    std::unordered_map<IOTV_Host* , QThread*> _iot_hosts;
    std::unordered_map<IOTV_Client*, QThread*> _iot_clients;

    QSettings _settingsServer, _settingsHosts;

    QString _address;
    quint16 _port;
    quint16 _broadcasrListenerPort;

    QTimer _reconnectTimer;

    uint _maxClientCount;
    uint _maxHostCount = 10;

    IOTV_Event_Manager *_eventManager;

    // широковещательный слушатель!
    QUdpSocket *_udpSocket;

private slots:
    void slotNewConnection();
    void slotDisconnected();

    void slotError(QAbstractSocket::SocketError error);

    void slotFetchEventActionData(QByteArray data);
    void slotQueryEventActionData();

    void slotPendingDatagrams();

    void slotBroadcastDevicePingTimeout();

    void slotTest();
};
