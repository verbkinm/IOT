#pragma once

#include <list>
#include <algorithm>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QByteArray>

#include "IOTV_Host/iotv_host.h"
#include "iotv_event_manager.h"

class IOTV_Client : public QObject
{
    Q_OBJECT
public:
    IOTV_Client(QTcpSocket *socket, const IOTV_Event_Manager *eventManager, const std::unordered_map<IOTV_Host* , QThread*> &hosts, QObject *parent);
    ~IOTV_Client();

    const QTcpSocket *socket() const;

    friend bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs);

private:
    QTcpSocket *_socket;
    const IOTV_Event_Manager *_eventManager;

    const std::unordered_map<IOTV_Host* , QThread*> &_hosts;

    QByteArray recivedBuff;

    QTimer _silenceTimer;

    static constexpr uint _silenceInterval = 6000;

    uint64_t _expectedDataSize;

    // Пришел запрос от клиента
    void processQueryIdentification();
    void processQueryState(const struct Header* header);
    void processQueryRead(const struct Header* header);
    void processQueryWrite(const struct Header* header);
    void processQueryPingPoing();
    void processQueryTech(const struct Header* header);

    void write(const QByteArray &data, qint64 size = -1) const;

private slots:
    void slotDisconnected();

    void slotReadData();

signals:
    void signalStopThread();
    void signalDisconnected();
};
