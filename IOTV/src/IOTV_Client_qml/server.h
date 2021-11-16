#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

#include <memory.h>
#include <map>

#include "device.h"
#include "Patterns/Subject.h"


class Server : public QObject, public Subject
{
    Q_OBJECT

    Q_PROPERTY(QString addr READ getServerAddress WRITE setServerAddres NOTIFY serverAddresChanged)
    Q_PROPERTY(quint16 port READ getServerPort WRITE setServerPort NOTIFY serverPortChanged)
    Q_PROPERTY(quint16 totalDevice READ getDevicesCount NOTIFY totalDeviceChanged)
    Q_PROPERTY(quint16 onlineDevice READ getDeviceOnline NOTIFY onlineDeviceChanged)

public:
    Server(QObject *parent = nullptr);
    ~Server();

    QString getName() const;
    QString getServerAddress() const;
    quint16 getServerPort() const;
    quint16 getDevicesCount() const;
    quint16 getDeviceOnline() const;

    void setName(const QString &name);
    void setServerAddres(const QString &serverAddress);
    void setServerPort(const quint16 &serverPort);
    void setServerNAP(const QString &name, const QString &serverAddress, const quint16 &serverPort);

    QAbstractSocket::SocketState state() const;

    qint64 writeData(QByteArray &data);

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
    std::map<QString, QString> _alias;

public slots:
    void connectToHost();
    void disconnectFromHost();

    void slotConnected();
    void slotDisconnected();

    void slotReadData();
    void slotError(QAbstractSocket::SocketError error);

signals:
    void serverAddresChanged(QString addr);
    void serverPortChanged(quint16 port);

    void totalDeviceChanged();
    void onlineDeviceChanged();

    void signalDevicesCreated();
    void signalDisconnected();
    void signalConnected();


};

#endif // SERVER_H
