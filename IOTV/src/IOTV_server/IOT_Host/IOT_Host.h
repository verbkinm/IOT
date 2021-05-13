#ifndef IOT_HOST_H
#define IOT_HOST_H

#include <memory>

#include "connection_type/ethernet_conn_type.h"
#include "connection_type/com_conn_type.h"
#include "base_host.h"

class IOT_Host : public Base_Host
{
    Q_OBJECT
public:
    IOT_Host(const QString &name, QObject* parent = nullptr);

    void printDebugData() const;

    void setConnectionTypeEthernet(const QString &addr, quint16 port);
    void setConnectionTypeCom(const QString &addr);

    void setInterval(uint interval);
    void setLogFile(const QString &logFile);

    QString getName() const override;
    QString getLogFile() const;

    Base_conn_type::Conn_type getConnectionType() const;

    bool isConnected() const override;

    int64_t readData(uint8_t channelNumber) override;
    int64_t writeData(uint8_t channelNumber, Raw::RAW rawData) override;

    virtual void dataResived(QByteArray data) override;

    void connectToHost();

private:
    void connectObjects() const;
    void response_WAY_recived(const QByteArray &data);
    void response_READ_recived(const QByteArray &data);
    void response_WRITE_recived(const QByteArray &data);

    std::unique_ptr<Base_conn_type> _host;
    QString _logFile;

    QTimer _intervalTimer;

    enum Flag
    {
        DeviceRegistered = 0x01,
        ExpectedWay = 0x02
    };

public:
    Q_DECLARE_FLAGS(Flags, Flag)

private:
    Flags _state;

private slots:
    void slotConnected();
    void slotDisconnected();

    void slotResendData();

    void slotTimeOut();

signals:
    void signalHostConnected();
    void signalHostDisconnected();

    void signalDataRiceved();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IOT_Host::Flags)

#endif // IOT_HOST_H

