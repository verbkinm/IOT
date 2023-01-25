#pragma once

#include <QTimer>

#include "log.h"
//#include "IOTV_SH.h"

class Base_conn_type : public QObject
{
    Q_OBJECT
public:
    Base_conn_type(const QString& name, QObject* parent);
    virtual ~Base_conn_type() = default;

    enum class Conn_type
    {
        NONE,
        COM,
        TCP,
        UDP,
        FILE
    };

    static constexpr int BUFFER_MAX_SIZE = 256;
    static constexpr int DEFAULT_INTERVAL = 10000; // таймер неудавшегося подключения

    QString getName() const;
    QString getAddress() const;
    Conn_type getConnectionType() const;

    void setAddress(const QString &address);

    virtual qint64 write(const QByteArray &data);
    virtual void connectToHost() = 0;
    virtual void disconnectFromHost() = 0;

    void clearDataBuffer();
    void setDataBuffer(QByteArray &data);

    static QString ConnTypeToString(Conn_type conn_type);

protected:
    const QString _name;
    QString _address;
    QString _logFile;
    Conn_type _type;
    QTimer _reconnectTimer;
    QByteArray _host_buffer_data;

    std::mutex _hostBuffMutex;

    virtual QByteArray readAll();

protected slots:
    void slotReadData();

signals:
    void signalConnected();
    void signalDisconnected();

    void signalDataRiceved(QByteArray);
};
