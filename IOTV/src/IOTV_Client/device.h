#ifndef DEVICE_H
#define DEVICE_H

#include "base_host.h"

class Device : public Base_Host
{
    Q_OBJECT
public:
    Device(const QString &name, QObject *parent = nullptr);

    virtual QString getName() const override;

    virtual bool isConnected() const override;

    virtual int64_t readData(uint8_t channelNumber) override;
    virtual int64_t writeData(uint8_t channelNumber, Raw::RAW rawData) override;
    virtual void dataResived(QByteArray data) override;

    void setState(bool state);

private:
    const QString _name;
    bool _state;

signals:
    void signalRead(uint8_t);
    void signalWrite(uint8_t, Raw::RAW);
};

#endif // DEVICE_H
