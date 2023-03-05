#pragma once

#include <list>
#include <algorithm>

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QByteArray>

#include "log.h"
#include "IOTV_Host/iotv_host.h"
#include "IOTV_SH.h"
#include "creatorpkgs.h"

class IOTV_Client : public QObject
{
    Q_OBJECT
public:
    IOTV_Client(QTcpSocket *socket, const std::unordered_map<IOTV_Host* , QThread*> &hosts, QObject *parent = nullptr);
    ~IOTV_Client();

    const QTcpSocket *socket() const;

    friend bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs);

private:
    QTcpSocket *_socket;

    const std::unordered_map<IOTV_Host* , QThread*> &_hosts;

    QByteArray recivedBuff;

    QTimer _silenceTimer;

    static constexpr uint _silenceInterval = 6000;

    uint64_t _expectedDataSize;

    // Пришел запрос от клиента
    void queryIdentification();
    void queryState(const struct Header* header);
    void queryRead(const struct Header* header);
    void queryWrite(const struct Header* header);
    void queryPingPoing();

    void write(const QByteArray &data, qint64 size = -1) const;

private slots:
    void slotDisconnected();

    void slotReadData();

signals:
    void signalStopThread();
    void signalDisconnected();
};
