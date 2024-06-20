#pragma once

#include <QDir>
#include <QDate>

#include <memory>
#include <mutex>
#include <map>
#include <set>

#include "raii_header.h"
#include "raii_iot.h"

#include "connection_type/tcp_conn_type.h"
#include "connection_type/tcp_reverse_conn_type.h"
#include "connection_type/udp_conn_type.h"
#include "connection_type/com_conn_type.h"
#include "connection_type/file_conn_type.h"

#include "base_host.h"
#include "IOTV_SH.h"

class IOTV_Host : public Base_Host
{
    Q_OBJECT
public:
    IOTV_Host(const std::unordered_map<QString, QString> &settingsData, QObject* parent = nullptr);
    IOTV_Host(const std::unordered_map<QString, QString> &settingsData, QTcpSocket *reverse_socket, QObject* parent = nullptr);
    virtual ~IOTV_Host();

    virtual QString getName() const override;
    virtual void setName(const QString &name) override;

    qint64 write(uint8_t channelNumber, QByteArray data);
    QByteArray readData(uint8_t channelNumber) const;

    const std::unordered_map<QString, QString> &settingsData() const;

    bool addStreamRead(uint8_t channel, const QString &client);
    void addStreamWrite(uint8_t channel);

    void removeStreamRead(uint8_t channel, const QString &client);
    void removeStreamWrite(uint8_t channel);

    QString getAddress() const;

    inline QString logName(const QDate &date)
    {
        return QFileInfo(_logDir, date.toString("yyyy-MM-dd")).absoluteFilePath() + ".log";
    }

    inline QString logName(const QDate &date, uint8_t channelNumber)
    {
        return QFileInfo(_logDir, date.toString("yyyy-MM-dd")).absoluteFilePath() + "_" + QString::number(channelNumber) + ".log";
    }

    inline QString logName()
    {
        return QFileInfo(_logDir, QDate::currentDate().toString("yyyy-MM-dd")).absoluteFilePath() + ".log";
    }

    inline QString logName(uint8_t channelNumber)
    {
        return QFileInfo(_logDir, QDate::currentDate().toString("yyyy-MM-dd")).absoluteFilePath() +  "_" + QString::number(channelNumber) + ".log";
    }

private:
    void shareConstructor();
    qint64 read(uint8_t channelNumber, readwrite_flag_t flags = ReadWrite_FLAGS_NONE);
    qint64 readAll();
    qint64 writeToRemoteHost(const QByteArray &data, qint64 size = -1);

    void makeConnType();

    void responceIdentification(RAII_Header raii_header);
    void responceState(RAII_iot raii_iot);
    void responceRead(RAII_Header raii_header);
    void responceWrite(RAII_iot raii_iot) const;
    void responcePingPong(RAII_iot raii_iot);

    std::unique_ptr<Base_conn_type> _conn_type;
    QDir _logDir;
    QTimer _timerReRead, _timerState, _timerPing;

    std::unordered_map<QString, QString>  _settingsData;

    // Что бы не плодить таймеры. Если отправляется пакет статуса уже N-ый раз, значит ответов не было и статус офлайн
    static constexpr int COUNTER_STATE_COUNT = 3;
    int _counterState;
    // Тоже самое для ping пакетов
    static constexpr int COUNTER_PING_COUNT = 3;
    int _counterPing;

    std::map<uint8_t, std::set<QString>> _streamRead, _streamWrite;

    QByteArray _buff;

public slots:
    void slotDisconnected();

    // дублирование функций addStreamRead  и removeStreamRead
    void slotAddStreamRead(uint8_t channel, QString client);
    void slotRemoveStreamRead(uint8_t channel, QString client);

private slots:
    void slotDataResived(QByteArray data);

    void slotReReadTimeOut();
    void slotStateTimeOut();
    void slotPingTimeOut();

    // Используетеся для записи данных полученых от клиентов из других потоков
    void slotQueryWrite(int channelNumber, QByteArray data);

    void slotConnected();

    void slotMoveToThread(QThread *newThread);

signals:
    void signalMoveToThread(QThread *newThread);
    void signalMovedToThread(QThread *oldThread);

    // Высылается из iotv_client
    void signalAddStreamRead(uint8_t channel, QString client);
    // Высылается из iotv_client
    void signalRemoveStreamRead(uint8_t channel, QString client);

    void signalDevicePingTimeOut();
    // Принимается в iotv_server
    void signalStreamRead(RAII_Header raii_header);
};
