#pragma once

#include <memory>
#include <mutex>

#include <QThread>

#include "connection_type/tcp_conn_type.h"
#include "connection_type/udp_conn_type.h"
#include "connection_type/com_conn_type.h"
#include "connection_type/file_conn_type.h"
#include "base_host.h"
#include "ConfigTypes.h"
#include "IOTV_SH.h"

class IOTV_Host : public Base_Host
{
    Q_OBJECT
public:
    IOTV_Host() = default;
    IOTV_Host(std::unordered_map<QString, QString> &settingsData, QObject* parent = nullptr);
    ~IOTV_Host();

    QString getName() const override;

    virtual bool isOnline() const override;

    qint64 write(uint8_t channelNumber, const QByteArray &data);
    QByteArray readData(uint8_t channelNumber) const;

    bool runInNewThread();

    const std::unordered_map<QString, QString> &settingsData() const;

private:
    qint64 read(uint8_t channelNumber);
    void dataResived(QByteArray data);
    qint64 writeToRemoteHost(const QByteArray &data, qint64 size = -1);

    void connectToHost();

    void setConnectionType();

    void setOnline(bool state);

    void responceIdentification(const struct Header *header);
    void responceState(struct IOTV_Server_embedded *iot);
    void responceRead(const struct Header* header);
    void responceWrite(struct IOTV_Server_embedded *iot);
    void responcePingPoing(struct IOTV_Server_embedded *iot);

    static constexpr uint16_t TIMER_PING = 10000;
    static constexpr uint16_t TIMER_PONG = 15000;

    std::unique_ptr<Base_conn_type> _conn_type;
    const QString _logFile;

    QTimer _timerReRead, _timerPing, _timerPong;

    std::unordered_map<QString, QString>  _settingsData;

    QThread _thread, *_parentThread;
    std::mutex _mutexParametersChange, _mutexWrite;

    uint64_t _expectedDataSize;

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

    void slotQueryWrite(int channelNumber, QByteArray data);

signals:
    void signalDataRiceved();

    void signalStopThread();

    void signalQueryWrite(int channelNumber, QByteArray data);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IOTV_Host::Flags)
