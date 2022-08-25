#pragma once

#include <memory>
#include <mutex>

#include "connection_type/tcp_conn_type.h"
#include "connection_type/com_conn_type.h"
#include "connection_type/file_conn_type.h"
#include "base_host.h"
#include "ConfigTypes.h"

#include <QThread>

class IOTV_Host : public Base_Host
{
    Q_OBJECT
public:
    IOTV_Host() = default;
    IOTV_Host(std::unordered_map<QString, QString> &settingsData, QObject* parent = nullptr);
    ~IOTV_Host();

    QString getName() const override;

    virtual bool isOnline() const override;

    virtual qint64 write(uint8_t channelNumber, const QByteArray &data) override;
    QByteArray readData(uint8_t channelNumber) const;

    bool runInNewThread();

private:
    virtual qint64 read(uint8_t channelNumber) override;
    virtual void dataResived(QByteArray data) override;
    virtual qint64 writeToRemoteHost(const QByteArray &data) override;

    void connectToHost();

    void setConnectionType();

    void setOnline(bool state);

    void response_WAY_recived(const IOTV_SH::RESPONSE_PKG *pkg);
    void response_READ_recived(const IOTV_SH::RESPONSE_PKG *pkg);
    void response_WRITE_recived(const IOTV_SH::RESPONSE_PKG *pkg);
    void response_PONG_recived(const IOTV_SH::RESPONSE_PKG *pkg);

    static constexpr uint16_t TIMER_PING = 10000;
    static constexpr uint16_t TIMER_PONG = 15000;

    std::unique_ptr<Base_conn_type> _conn_type;
    const QString _logFile;

    QTimer _reReadTimer, _timerPing, _timerPong;

    std::unordered_map<QString, QString>  _settingsData;

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

    void signalStopThread();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IOTV_Host::Flags)
