#pragma once

#include <list>
#include <algorithm>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QByteArray>
#include <queue>

#include "IOTV_Host/iotv_host.h"
#include "thread_pool.h"
#include "raii_header.h"
#include "raii_iot.h"

class IOTV_Client : public QObject
{
    Q_OBJECT
public:
    IOTV_Client(QTcpSocket *socket, QObject *parent = nullptr);
    ~IOTV_Client();

    const QTcpSocket *socket() const;

    friend bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs);

private:
    void processQueryLogData(RAII_Header header, QString fileName, bool hostError, std::atomic_int &run);

    void write(const QByteArray &data, qint64 size = -1) const;

    static uint64_t writeFunc(char *data, uint64_t size, void *obj);

    QTcpSocket *_socket;

    QByteArray _recivedBuff;

    QTimer _silenceTimer;
    static constexpr uint _SILENCEINTERVAL = 15000;

    uint64_t _expectedDataSize;

    std::mutex _logDataQueueMutex;
    std::queue<std::pair<RAII_Header, std::vector<char>>> _logDataQueue;
    QTimer _logDataQueueTimer;
    static constexpr uint _LOGDATAQUEUETIMERINTERVAL = 100;

    thread_pool::ThreadPool *_my_pool;

public slots:
     void slotStreamRead(uint8_t channel, uint16_t fragment, uint16_t fragments, QByteArray data);

private slots:
    void slotDisconnected();

    void slotReadData();
    void slotFetchEventActionDataFromServer(QByteArray data);



    void slotLogDataQueueTimerOut();

    void slotUpdateHosts(QByteArray data);


    void slotServerToClientQueryIdentification(QByteArray data);
    void slotServerToClientQueryRead(RAII_Header raii_header, RAII_iot raii_iot);
    void slotServerToClientQueryWrite(RAII_Header raii_header, RAII_iot raii_iot);
    void slotServerToClientQueryState(RAII_iot raii_iot);
    void slotServerToClientQueryLogData(RAII_Header raii_header, QString logName, bool hostError);

signals:
    void signalStopThread();
    void signalDisconnected();

    void signalFetchEventActionDataFromServer(QByteArray data);

    // Высылается сервером, при добавлении новых устройств или удалении
    void signalUpdateHosts(QByteArray data);

    // Все сигналы отлавливает IOTV_Server. IOTV_Server очищает полученные header_t * !!!
    void signalClientToServerQueryIdentification();
    void signalClientToServerQueryRead(RAII_Header raii_header);
    void signalClientToServerQueryWrite(RAII_Header raii_header);
    void signalClientToServerQueryState(RAII_Header raii_header);
    //пинг остаётся на стороне IOTV_Client, так как ответ не требует чтение данных хостов
    void signalClientToServerQueryTech(RAII_Header raii_header);
    void signalClientToServerLogData(RAII_Header raii_header);

    // Ответ IOTV_Server на сигналы ClientToServer
    void signalServerToClientQueryIdentification(QByteArray data);
    void signalServerToClientQueryRead(RAII_Header raii_header, RAII_iot raii_iot);
    void signalServerToClientQueryWrite(RAII_Header raii_header, RAII_iot raii_iot);
    void signalServerToClientQueryState(RAII_iot raii_iot);
    void signalServerToClientQueryLogData(RAII_Header raii_header, QString logName, bool hostError);
};
