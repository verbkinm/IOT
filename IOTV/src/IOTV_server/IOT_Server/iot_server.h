#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <QSettings>
#include <QTcpServer>
#include <QFileInfo>
#include <QTimer>

#include <vector>
#include <list>
#include <memory>

#include "protocols.h"
#include "IOT_Host/IOT_Host.h"

//#define DEFAULT_INTERVAL 5000

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

    std::vector<std::shared_ptr<IOT_Host>> _iot_hosts;
    std::list<QTcpSocket*> _clientList;
    QSettings _settingsServer, _settingsHosts;
    const QString _programVersion;
    QString _address;
    quint16 _port;
    QString _logFile;
    QTimer _reconnectTimer;

signals:

private slots:
    void slotNewConnection();
    void slotDataRecived();

    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError error);

    void slotResponse_Way();
};

#endif // IOT_SERVER_H
