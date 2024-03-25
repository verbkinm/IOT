#pragma once

#include "log.h"
#include "raw.h"
#include "creatorpkgs.h"

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
        TCP_REVERSE,
        UDP,
        UDP_REVERSE,
        FILE
    };

    static constexpr int BUFFER_MAX_SIZE = BUFSIZ;

    QString getName() const;
    QString getAddress() const;
    Conn_type getConnectionType() const;

    void setAddress(const QString &address);

    virtual qint64 write(const QByteArray &data, qint64 size = -1) = 0;
    virtual void connectToHost() = 0;
    virtual void disconnectFromHost() = 0;

    static QString ConnTypeToString(Conn_type conn_type);

    uint64_t expectedDataSize;

protected:
    const QString _name;
    QString _address;
    QString _logFile;
    Conn_type _type;
//    QByteArray _host_buffer_data;

    std::mutex _hostBuffMutex;

    virtual QByteArray readAll();

protected slots:
    void slotReadData();

signals:
    void signalConnected();
    void signalDisconnected();

    void signalDataRiceved(QByteArray);
};
