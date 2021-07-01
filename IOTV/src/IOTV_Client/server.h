#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QHostAddress>
#include <GUI/gui_server.h>

#include <memory.h>
#include <map>

#include "device.h"
#include "Patterns/Subject.h"
#include "GUI/devicelist.h"


class Server : public QObject, public Subject
{
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);
    ~Server();

    QString getName() const;
    QString getServerAddress() const;
    uint16_t getServerPort() const;
    uint getDevicesCount() const;
    uint getDeviceOnline() const;

    void setName(const QString &name);
    void setServerAddres(const QString &serverAddress);
    void setServerPort(const uint16_t &serverPort);
    void setServerNAP(const QString &name, const QString &serverAddress, const uint16_t &serverPort);

    QAbstractSocket::SocketState state() const;

    void connectToHost();
    void disconnectFromHost();

    qint64 writeData(QByteArray &data);

    void deviceListShow(const QIcon &windowIcon);

    const std::map<QString, std::shared_ptr<Device> > &getDevices() const;

    void addAlias(const QString &name, const QString &aliasName);

private:
    void createDevice(QByteArray &data);
    void newObjectName();

    QTcpSocket _socket;
    QString _name;
    QString _serverAddress;
    uint16_t _serverPort;
    QTimer _reconnectTimer;
    uint8_t _reconnectTimerTrying;

    std::map<QString, std::shared_ptr<Device>> _devices;
    std::unique_ptr<DeviceList> _deviceList;
    std::map<QString, QString> _alias;

private slots:
    void slotConnected();
    void slotDisconnected();

    void slotReadData();
    void slotError(QAbstractSocket::SocketError error);

signals:
    void signalDeviceCreated();

};

#endif // SERVER_H
