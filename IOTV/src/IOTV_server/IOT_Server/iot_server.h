#pragma once

#include <QSettings>
#include <QTcpServer>
#include <QFileInfo>
#include <QTimer>
#include <QThread>

#include <vector>
#include <list>
#include <memory>
#include <fstream>

#include "protocols.h"
#include "IOT_Host/IOT_Host.h"
#include "IOT_Host/iot_host_structsettings.h"

class IOT_Server : public QTcpServer
{
    Q_OBJECT
public:
    IOT_Server(QObject *parent = nullptr);
    ~IOT_Server();

    QStringList getFileSettingNames() const;
    QString getProgramVersion() const;

private:
    void checkSettingsFileExist();
    void readSettings();
    void startTCPServer();

    void writeToSocket(QTcpSocket* socket, const QByteArray &data);
    void clinetOnlineFile() const;

    std::vector<std::unique_ptr<IOT_Host>> _iot_hosts;
    std::list<QTcpSocket*> _clientList;

    QSettings _settingsServer, _settingsHosts;

    const QString _programVersion;
    QString _address;
    quint16 _port;
    QString _logFile;

    QTimer _reconnectTimer;

    QByteArray _server_buffer_data;

private slots:
    void slotNewConnection();
    void slotDataRecived();

    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError error);

    void slotResponse_Way();
};
