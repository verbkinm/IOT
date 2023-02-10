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

#include "iotvp_print.h"

class IOTV_Host : public Base_Host
{
    Q_OBJECT
public:
    IOTV_Host() = default;
    IOTV_Host(std::unordered_map<QString, QString> &settingsData, QObject* parent = nullptr);
    ~IOTV_Host();

    QString getName() const override;

    qint64 write(uint8_t channelNumber, const QByteArray &data);
    QByteArray readData(uint8_t channelNumber) const;

    bool runInNewThread();

    const std::unordered_map<QString, QString> &settingsData() const;

private:
    qint64 read(uint8_t channelNumber);
    qint64 writeToRemoteHost(const QByteArray &data, qint64 size = -1);

    void setConnectionType();

    void responceIdentification(const struct Header *header);
    void responceState(const struct IOTV_Server_embedded *iot);
    void responceRead(const struct Header* header);
    void responceWrite(const struct IOTV_Server_embedded *iot) const;
    void responcePingPoing(const struct IOTV_Server_embedded *iot);

    std::unique_ptr<Base_conn_type> _conn_type;
    const QString _logFile;
    QTimer _timerReRead, _timerState, _timerDeviceUnavailable;

    std::unordered_map<QString, QString>  _settingsData;

    QThread _thread, *_parentThread;
    std::mutex _mutexParametersChange, _mutexWrite;

private slots:
    void slotDataResived(QByteArray data);

    void slotReReadTimeOut();
    void slotStateTimeOut();
    void slotDeviceUnavailableTimeOut();

    void slotNewThreadStart();
    void slotThreadStop();

    // Используетеся для записи данных полученых от клиентов из других потоков
    void slotQueryWrite(int channelNumber, QByteArray data);

signals:
    void signalDeviceUnavailableTimeOut();
    void signalDataRiceved();

    void signalStopThread();

    // Используетеся для записи данных полученых от клиентов из других потоков
    void signalQueryWrite(int channelNumber, QByteArray data);
};
