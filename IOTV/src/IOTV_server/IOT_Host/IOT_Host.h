#pragma once

#include <memory>
#include <mutex>

#include <QThread>

#include "connection_type/tcp_conn_type.h"
#include "connection_type/com_conn_type.h"
#include "connection_type/file_conn_type.h"
#include "iot_host_structsettings.h"

class IOT_Host : public Base_Host
{
    Q_OBJECT

public:
    IOT_Host(IOT_Host_StructSettings &structSettings, QObject* parent = nullptr);
    ~IOT_Host();

    void printDebugData() const;

    //!!!
    //    void setConnectionType();


    QString getName() const override;
    QString getLogFile() const;

    Base_conn_type::Conn_type getConnectionType() const;

    virtual void setOnline(bool state) override;
    virtual bool isOnline() const override;

    virtual qint64 readData(uint8_t channelNumber) override;
    virtual qint64 writeData(uint8_t channelNumber, Raw::RAW &rawData) override;
    virtual qint64 writeToServer(QByteArray &data) override;

    virtual void dataResived(QByteArray data) override;

    void connectToHost();

    bool runInNewThread();

private:
    void setConnectionTypeTCP(const QString &addr, quint16 port);
    void setConnectionTypeCom(const QString &addr, const COM_conn_type::SetingsPort &settingPort);
    void setConnectionTypeFile(const QString &addr);

    void setInterval(uint interval);
    void setLogFile(const QString &logFile);

    void connectObjects() const;

    void response_WAY_recived(const QByteArray &data);
    void response_READ_recived(const QByteArray &data);
    void response_WRITE_recived(const QByteArray &data);
    void response_PONG_recived();

    static constexpr unsigned int TIMER_PING = 10000;
    static constexpr unsigned int TIMER_RECONNECT = 15000;

    std::unique_ptr<Base_conn_type> _conn_type;
    QString _logFile;

    QTimer _reReadTimer, _timerPing, _timerReconnect;

    std::mutex _mutexParametersChange;
    QThread _thread;

    IOT_Host_StructSettings _structSettings;

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

signals:
    void signalHostConnected();
    void signalHostDisconnected();

    void signalDataRiceved(); // !!!

    void signalResponse_Way();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IOT_Host::Flags)
