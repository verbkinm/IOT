#ifndef DEVICE_H
#define DEVICE_H

#include "GUI/Devices/gui_base_device.h"
#include "Patterns/Subject.h"
#include "base_host.h"
#include "protocols.h"

class Server;

class Device : public Base_Host, public Subject
{
    Q_OBJECT
public:
    Device(Server &server, const QString &name, uint8_t id, QObject *parent = nullptr);
    ~Device();

    virtual QString getName() const override;

    virtual qint64 readData(uint8_t channelNumber) override;
    virtual qint64 writeData(uint8_t channelNumber, Raw::RAW &rawData) override;
    virtual void dataResived(QByteArray data) override;

    virtual void setOnline(bool state) override;
    virtual bool isOnline() const override;

//    virtual void setRegistered(bool state) override;
//    virtual bool isRegistered() const override;

    int getAutoReadInterval() const;
    void setAutoReadInterval(int value);

    virtual qint64 writeToServer(QByteArray &data) override;

    void setAutoReadEnable(bool state);

    QString getViewName() const;
    void setViewName(const QString &viewName);

    QString getServerObjectName() const;

private:
    void newObjectName();

    const QString _name;
    QString _viewName;
    bool _state;
    Server &_server;

    QTimer _autoReadInterval, _stateInterval;

private slots:
    void slotAutoReadTimeOut();
    void slotStateIntervalTimeOut();
};

#endif // DEVICE_H
