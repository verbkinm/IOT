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
#include <algorithm>

#include "IOTV_Host/iotv_host.h"
#include "IOTV_Client/iotv_client.h"

#include "event_action_parser.h"
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

    void clientOnlineFile() const;

    Base_Host *baseHostFromName(const QString &name) const;

    //! Возвращает список Base_Host* из _iot_hosts
    std::forward_list<const Base_Host *> baseHostList() const;
    //    void parseJson(const QByteArray &data);
    //    IOTV_Event *parseEvent(const QJsonObject &jobj) const;
    //    IOTV_Action *parseAction(const QJsonObject &jobj) const;
    //    QByteArray toData() const;

    std::unordered_map<IOTV_Host* , QThread*> _iot_hosts;
    std::unordered_map<IOTV_Client*, QThread*> _iot_clients;

    QSettings _settingsServer, _settingsHosts;

    QString _address;
    quint16 _port;

    QTimer _reconnectTimer;

    uint _maxClientCount;

    IOTV_Event_Manager *_eventManager;

private slots:
    void slotNewConnection();
    void slotDisconnected();

    void slotError(QAbstractSocket::SocketError error);

    void slotFetchEventActionData(QByteArray data);
    void slotQueryEventActionData();

    void slotTest();
};
