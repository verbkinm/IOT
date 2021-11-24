#ifndef DEVICE_H
#define DEVICE_H

#include "Patterns/Subject.h"
#include "lib/Base_Host/base_host.h"
#include "lib/Protocols/protocols.h"

class Server;

class Device : public Base_Host, public Subject
{
    Q_OBJECT
public:
    Device(Server &server, const QString &name, uint8_t id, QObject *parent = nullptr);
    ~Device();

    Q_INVOKABLE quint8 getIdToQML() const;
    virtual QString getName() const override;
    virtual bool getState() const override;

    virtual qint64 readData(uint8_t channelNumber) override;
    virtual qint64 writeData(uint8_t channelNumber, Raw::RAW &rawData) override;
    virtual void dataResived(QByteArray data) override;

    virtual void setState(bool state) override;

    int getAutoReadInterval() const;
    void setAutoReadInterval(int value);

    virtual qint64 writeToServer(QByteArray &data) override;

    Q_INVOKABLE void setAutoReadEnable(bool state);

    Q_INVOKABLE QString getViewName() const;
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

signals:
    void signalState(bool state);
    void signalDataReadRecived();
    void signalRecreateDevices();
};

#endif // DEVICE_H
