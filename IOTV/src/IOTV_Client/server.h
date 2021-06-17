#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QPushButton>
#include <QScrollArea>
#include <GUI/gui_server.h>
#include <GUI/Devices/gui_device_creator.h>

#include "memory.h"
#include <algorithm>
#include "map"
#include <vector>

#include "device.h"
#include "Patterns/Subject.h"
#include "devicelist.h"

#define DEFAULT_INTERVAL 5000
#define RECONNECTING_COUNT 5

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

    QAbstractSocket::SocketState state() const;

    void connectToHost();
    void disconnectFromHost();

    qint64 writeData(QByteArray data);

    void deviceListShow();

    size_t deviceCount() const;

private:
    void createDevice(QByteArray &data);

    QTcpSocket _socket;
    QString _name;
    QString _serverAddress;
    uint16_t _serverPort;
    QTimer _reconnectTimer;
    uint8_t _reconnectTimerTrying;

    std::map<QString, std::unique_ptr<Device>> _devices;
    std::unique_ptr<DeviceList> _deviceList;

public slots:
    void slotDeleteForm();

private slots:
    void slotConnected();
    void slotDisconnected();

    void slotReadData();
    void slotError(QAbstractSocket::SocketError error);

};

#endif // SERVER_H
