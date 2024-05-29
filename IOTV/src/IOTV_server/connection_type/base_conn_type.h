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
        FILE
    };

    static constexpr int BUFFER_MAX_SIZE = BUFSIZ;

    QString getName() const;
    void setName(const QString &newName);

    QString getAddress() const;
    Conn_type getConnectionType() const;

    void setAddress(const QString &address);

    virtual qint64 write(const QByteArray &data, qint64 size = -1) = 0;
    virtual void connectToHost() = 0;
    virtual void disconnectFromHost() = 0;

    static QString ConnTypeToString(Conn_type conn_type);
    static bool isIpConnectionType(const QString &conn_type);

    uint64_t expectedDataSize;


protected:
    QString _name;
    QString _address;
    QString _logFile;
    Conn_type _type;

    virtual QByteArray readAll();

protected slots:
    void slotReadData();

signals:
    void signalConnected();
    void signalDisconnected();

    // отлавливается в iotv_hosts
    void signalDataRiceved(QByteArray);
};
