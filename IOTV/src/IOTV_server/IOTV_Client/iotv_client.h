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

class IOTV_Client : public QObject
{
    Q_OBJECT
public:
    IOTV_Client(QTcpSocket *socket, const std::unordered_map<IOTV_Host* , QThread*> &hosts, QObject *parent = nullptr);
    ~IOTV_Client();

    const QTcpSocket *socket() const;

    friend bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs);

private:
    // Пришел запрос от клиента
    void processQueryIdentification();
    // Удаление всех устройств, а потом обновление всего списка!
    void processQueryIdentification_2();

    void processQueryState(const header_t* header);
    void processQueryRead(const header_t* header);
    void processQueryWrite(const header_t* header);
    void processQueryPingPoing();
    void processQueryTech(const header_t* header);
    void processQueryLogData(header_t* header, std::atomic_int &run);

    void write(const QByteArray &data, qint64 size = -1) const;

    static uint64_t writeFunc(char *data, uint64_t size, void *obj);

    QTcpSocket *_socket;

    const std::unordered_map<IOTV_Host *, QThread *> &_hosts;

    QByteArray _recivedBuff;

    QTimer _silenceTimer;
    static constexpr uint _SILENCEINTERVAL = 15000;

    uint64_t _expectedDataSize;

    std::mutex _logDataQueueMutex;
    std::queue<std::pair<Header *, std::vector<char>>> _logDataQueue;
    QTimer _logDataQueueTimer;
    static constexpr uint _LOGDATAQUEUETIMERINTERVAL = 100;

    thread_pool::ThreadPool *_my_pool;


private slots:
    void slotDisconnected();

    void slotReadData();
    void slotFetchEventActionDataFromServer(QByteArray data);

    void slotStreamRead(uint8_t channel, uint16_t fragment, uint16_t fragments, QByteArray data);

    void slotLogDataQueueTimerOut();

signals:
    void signalStopThread();
    void signalDisconnected();

    void signalFetchEventActionData(QByteArray data);
    void signalQueryEventActionData(); // отлавливается в iotv_server
    void signalFetchEventActionDataFromServer(QByteArray data);

    // Высылается сервером, при добавлении новых устройств
    void signalUpdateHosts();
    // Высылается сервером, при уделении устройства
    void signalClearAndUpdateHosts();
};
