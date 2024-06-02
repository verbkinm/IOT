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
    // Пришел запрос от клиента
    void processQueryIdentification(QByteArray data);
    // Удаление всех устройств, а потом обновление всего списка!
    void processQueryIdentification_2();

    void processQueryState(const header_t* header);
    void processQueryRead(iotv_obj_t *iot);
    void processQueryWrite(const header_t* header);
    void processQueryPingPoing();
    void processQueryTech(const header_t* header);
    void processQueryLogData(header_t* header, std::atomic_int &run);

    void write(const QByteArray &data, qint64 size = -1) const;

    static uint64_t writeFunc(char *data, uint64_t size, void *obj);

    QTcpSocket *_socket;

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



    void slotServerToClientQueryIdentification(QByteArray data);
    void slotServerToClientQueryRead(header_t *header, iotv_obj_t *iot);

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

    // Все сигналы отлавливает IOTV_Server. IOTV_Server очищает полученные header_t * !!!
    void signalClientToServerQueryIdentification();
    void signalClientToServerQueryRead(RAII_Header raii_header);
    void signalClientToServerQueryWrite(RAII_Header raii_header);
    void signalClientToServerQueryState(header_t *header);
    //пинг остаётся на стороне IOTV_Client, так как ответ не требует чтение данных хостов
    void signalClientToServerQueryTech(header_t *header);
    void signalClientToServerLogData(header_t *header);

    // Ответ IOTV_Server на сигналы ClientToServer
    void signalServerToClientQueryIdentification(QByteArray data);
    void signalServerToClientQueryRead(RAII_Header raii_header, RAII_iot raii_iot);
};
