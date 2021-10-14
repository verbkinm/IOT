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
    void setConnectionTypeCom(const QString &addr, const COM_conn_type::SetingsPort &settingPort);

    void setInterval(uint interval);
    void setLogFile(const QString &logFile);

    QString getName() const override;
    QString getLogFile() const;

    Base_conn_type::Conn_type getConnectionType() const;

    virtual void setState(bool state) override;
    virtual bool getState() const override;


    virtual qint64 readData(uint8_t channelNumber) override;
    virtual qint64 writeData(uint8_t channelNumber, Raw::RAW &rawData) override;
    virtual qint64 writeToServer(QByteArray &data) override;

    virtual void dataResived(QByteArray data) override;

    void connectToHost();

private:
    void connectObjects() const;
    void response_WAY_recived(const QByteArray &data);
    void response_READ_recived(const QByteArray &data);
    void response_WRITE_recived(const QByteArray &data);

    std::unique_ptr<Base_conn_type> _conn_type;
    QString _logFile;

    QTimer _intervalTimer, _timerWAY;

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
    void slotWAYTimeOut();

signals:
    void signalHostConnected();
    void signalHostDisconnected();

    void signalDataRiceved(); //!!!

    void signalResponse_Way();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IOT_Host::Flags)

#endif // IOT_HOST_H

