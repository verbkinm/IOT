#ifndef DEVICE_H
#define DEVICE_H

#include "GUI/Devices/gui_base_device.h"
#include "Patterns/Subject.h"
#include "base_host.h"

class Device : public Base_Host, public Subject
{
    Q_OBJECT
public:
    Device(const QString &name, QObject *parent = nullptr);

    virtual QString getName() const override;
    virtual bool getState() const override;

    virtual int64_t readData(uint8_t channelNumber) override;
    virtual int64_t writeData(uint8_t channelNumber, Raw::RAW rawData) override;
    virtual void dataResived(QByteArray data) override;

    virtual void setState(bool state) override;

    int getAutoReadInterval() const;
    void setAutoReadInterval(int value);

private:
    const QString _name;
    bool _state;

    QTimer _autoReadInterval;

signals:
    void signalRead(uint8_t channelNumber);
    void signalWrite(uint8_t channelNumber, Raw::RAW raw);

private slots:
    void slotAutoReadTimeOut();
};

#endif // DEVICE_H
