#pragma once

#include <list>
#include <algorithm>

#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>

#include "log.h"
#include "IOTV_Host/iotv_host.h"
#include "IOTV_SC/IOTV_SC.h"

class IOTV_Client : public QObject
{
    Q_OBJECT
public:
    IOTV_Client(QTcpSocket *socket, std::list<IOTV_Host> &hosts, QObject *parent = nullptr);
    ~IOTV_Client();

    bool runInNewThread();

    const QTcpSocket *socket() const;

    friend bool operator==(const IOTV_Client &lhs, const IOTV_Client &rhs);

private:
    QThread _thread, *_parentThread;
    QTcpSocket *_socket;

    std::list<IOTV_Host> &_hosts;

    QByteArray recivedBuff;

    void query_DEV_LIST_recived(Server_RX::QUERY_PKG *pkg);
    void query_STATE_recived(Server_RX::QUERY_PKG *pkg);
    void query_READ_recived(Server_RX::QUERY_PKG *pkg);
    void query_WRITE_recived(Server_RX::QUERY_PKG *pkg);

private slots:
    void slotDisconnected();

    void slotNewThreadStart();
    void slotThreadStop();

    void slotReadData();

signals:
    void signalStopThread();
    void signalDisconnected();
};
