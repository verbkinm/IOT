#pragma once

#include <unordered_map>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QSettings>
#include <QFileInfo>

#include "log.h"
#include "IOTV_SC.h"
#include "device.h"

class Client : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString addr READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(quint16 totalDevice READ countDevices NOTIFY countDeviceChanged)
    Q_PROPERTY(quint16 onlineDevice READ countDeviceOnline NOTIFY onlineDeviceChanged)
    Q_PROPERTY(bool state READ stateConnection NOTIFY stateConnectionChanged)
    Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect NOTIFY autoConnectChanged)

public:
    Client(QObject *parent = nullptr);
    ~Client();

    qint64 writeData(const QByteArray &data);

    const QString &address() const;
    quint16 port() const;

    int countDevices() const;
    int countDeviceOnline() const;

    void setAddress(const QString &address);
    void setPort(quint64 port);

    QByteArray readData(const QString &deviceName, uint8_t channelNumber) const;

    Q_INVOKABLE QList<QObject*> devList();
    Q_INVOKABLE QObject *deviceByName(QString name);

    bool stateConnection() const;

    bool autoConnect() const;
    void setAutoConnect(bool newAutoConnect);

private:
    QTcpSocket _socket;
    QByteArray _recivedBuff;

    QTimer _timerDevList;
    QTimer _reconnectTimer;
    QTimer _connectWait;

    QString _address;
    quint16 _port;

    bool _stateConnection;
    bool _autoConnect;

    std::unordered_map<QString, Device> _devices;

    QSettings _settingClient;

    void response_DEV_LIST(IOTV_SC::RESPONSE_PKG *pkg);
    void response_STATE(IOTV_SC::RESPONSE_PKG *pkg);
    void response_READ(IOTV_SC::RESPONSE_PKG *pkg);
    void response_WRITE(IOTV_SC::RESPONSE_PKG *pkg) const;

    void write(const QByteArray &data);

    void setStateConnection(bool newStateConnection);

    void checkSettingsFileExist();
    void readSettings();
    void saveSettings();


public slots:
    void connectToHost();
    void disconnectFromHost();

    void slotConnected();
    void slotDisconnected();

private slots:
    void slotReciveData();

    void slotQueryRead();
    void slotQueryState();
    void slotQueryDevList();

    void slotError(QAbstractSocket::SocketError error);
    void slotConnectWait();

signals:
    void signalConnected();
    void signalDisconnected();

    void addressChanged(QString);
    void portChanged(quint64);

    void countDeviceChanged();
    void onlineDeviceChanged();
    void stateConnectionChanged();
    void autoConnectChanged();
    void signalConnectWait();
};
