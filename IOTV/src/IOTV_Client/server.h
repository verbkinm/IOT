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
    quint16 getServerPort() const;
    uint getDevicesCount() const;
    uint getDeviceOnline() const;

    void setName(const QString &name);
    void setServerAddres(const QString &serverAddress);
    void setServerPort(const quint16 &serverPort);
    void setServerNAP(const QString &name, const QString &serverAddress, const quint16 &serverPort);

    QAbstractSocket::SocketState state() const;

    void connectToHost();
    void disconnectFromHost();

    qint64 writeData(QByteArray &data);

    void deviceListShow();

    const std::map<QString, std::shared_ptr<Device> > &getDevices() const;

    void addAlias(const QString &name, const QString &aliasName);
    const std::map<QString, QString> &getAlias() const;

private:
    void createDevice(const QByteArray &data, const QString &name);
    void newObjectName();

    QTcpSocket _socket;
    QString _name;
    QString _serverAddress;
    quint16 _serverPort;
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
    void signalDevicesCreated();
    void signalDisconnected();

};

#endif // SERVER_H
