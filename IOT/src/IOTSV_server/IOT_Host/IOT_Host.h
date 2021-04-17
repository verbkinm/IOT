#ifndef IOT_HOST_H
#define IOT_HOST_H

#include <memory>

#include "connection_type/ethernet_conn_type.h"
#include "connection_type/com_conn_type.h"
#include "channels/channelmanager.h"
#include "shedule.h"

class IOT_Host : public QObject
{
    Q_OBJECT
public:
    IOT_Host(const QString &name, QObject* parent = nullptr);
    virtual ~IOT_Host();

    void printDebugData() const;

    void setConnectionType(Base_conn_type::Conn_type conn_type);
    void setConnectionTypeEthernet(const QString &addr, quint16 port);
    void setConnectionTypeCom(const QString &addr);

    void setUser_description(const QString &userDescription);
    void setName(const QString &name);
    void setId(uint8_t id);
    void setDescription(const QString &description);

    QString getName() const;
    uint8_t getId() const;
    QString getDescription() const;
    QString getUser_description() const;    

    Base_conn_type::Conn_type getConnectionType() const;

    qint64 readData(uint8_t channelNumber);
    qint64 writeData(uint8_t channelNumber, Raw::RAW rawData);

    void connectToHost();

    ChannelManager channelManager;

private:
    void connectObjects() const;

    uint8_t _id;             // from device
    QString _description;     // from device
    QString _userDescription;// from device

    std::unique_ptr<Base_conn_type> _host;

    std::vector<Shedule> _shedule;

    enum Flag
    {
        DeviceRegistered = 0x1,
        ExpectedWay = 0x2,
    };

public:
    Q_DECLARE_FLAGS(Flags, Flag)

private:
    Flags _state;

private slots:
    void slotConnected();
    void slotDisconnected();

    void slotDataRiceved(QByteArray data);
    void slotResendData();

    void slotShedule();

signals:
    void signalDataRiceved();

};

Q_DECLARE_OPERATORS_FOR_FLAGS(IOT_Host::Flags)

#endif // IOT_HOST_H

