#pragma once

#include <memory>
#include <mutex>

#include "connection_type/tcp_conn_type.h"
#include "connection_type/com_conn_type.h"
#include "connection_type/file_conn_type.h"
#include "base_host.h"
#include "iot_host_structsettings.h"

#include <QThread>

class IOT_Host : public Base_Host
{
    Q_OBJECT
public:
    IOT_Host(IOT_Host_StructSettings *structSettings, QObject* parent = nullptr);
    ~IOT_Host();

    QString getName() const override;
    QString getLogFile() const;

    Base_conn_type::Conn_type getConnectionType() const;

    virtual bool isOnline() const override;

    virtual qint64 writeData(uint8_t channelNumber, const QByteArray &data) override;
    Raw data(uint8_t channelNumber) const;

    bool runInNewThread();

private:
    virtual qint64 readData(uint8_t channelNumber) override;
    virtual void dataResived(QByteArray data) override;
    virtual qint64 writeToServer(const QByteArray &data) override;

    void connectToHost();

    void setConnectionTypeTCP();
    void setConnectionTypeCom();
    void setConnectionTypeFile();

    void setInterval(uint interval);
    void setLogFile(const QString &logFile);

    void setOnline(bool state);

    void connectObjects() const;

    void response_WAY_recived(const IOTV_SH::RESPONSE_PKG *pkg);
    void response_READ_recived(const IOTV_SH::RESPONSE_PKG *pkg);
    void response_WRITE_recived(const IOTV_SH::RESPONSE_PKG *pkg);
    void response_PONG_recived(const IOTV_SH::RESPONSE_PKG *pkg);

    static constexpr unsigned int TIMER_PING = 10000;
    static constexpr unsigned int TIMER_RECONNECT = 15000;

    std::unique_ptr<Base_conn_type> _conn_type;
    QString _logFile;

    QTimer _reReadTimer, _timerPing, _timerReconnect;

    IOT_Host_StructSettings *_structSettings;

    QThread _thread, *_parentThread;
    std::mutex _mutexParametersChange, _mutexWrite;

    enum Flag
    {
        DeviceRegistered = 0x01,
        DeviceOnline = 0x02
    };

public:
    Q_DECLARE_FLAGS(Flags, Flag)

private:
    Flags _state_flags;

private slots:
    void slotConnected();
    void slotDisconnected();

    void slotReReadTimeOut();
    void slotPingTimeOut();
    void slotReconnectTimeOut();

    void slotNewThreadStart();
    void slotThreadStop();

signals:
    void signalHostConnected();
    void signalHostDisconnected();

    void signalDataRiceved();

    void signalResponse_Way();

    void signalStopThread();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IOT_Host::Flags)
