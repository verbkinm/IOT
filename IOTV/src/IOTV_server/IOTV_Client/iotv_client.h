#pragma once

#include <list>
#include <algorithm>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QByteArray>

#include "IOTV_Host/iotv_host.h"

class IOTV_Client : public QObject
{
    Q_OBJECT
public:
    IOTV_Client(QTcpSocket *socket, const std::unordered_map<IOTV_Host* , QThread*> &hosts, QObject *parent);
    ~IOTV_Client();

    const QTcpSocket *socket() const;

    friend bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs);

private:
    QTcpSocket *_socket;

    const std::unordered_map<IOTV_Host *, QThread *> &_hosts;

    QByteArray _recivedBuff;

    QTimer _silenceTimer;

    static constexpr uint _silenceInterval = 15000;

    uint64_t _expectedDataSize;

    // Пришел запрос от клиента
    void processQueryIdentification();
    void processQueryState(const struct Header* header);
    void processQueryRead(const struct Header* header);
    void processQueryWrite(const struct Header* header);
    void processQueryPingPoing();
    void processQueryTech(const struct Header* header);

    void write(const QByteArray &data, qint64 size = -1) const;

    static uint64_t writeFunc(char *data, uint64_t size, void *obj);

private slots:
    void slotDisconnected();

    void slotReadData();
    void slotFetchEventActionDataFromServer(QByteArray data);

    void slotStreamRead(uint8_t channel, uint16_t fragment, uint16_t fragments, QByteArray data);

signals:
    void signalStopThread();
    void signalDisconnected();

    void signalFetchEventActionData(QByteArray data);
    void signalQueryEventActionData();
    void signalFetchEventActionDataFromServer(QByteArray data);

    // Высылается сервером, при добавлении новых устройств
    void signalUpdateHosts();

};
