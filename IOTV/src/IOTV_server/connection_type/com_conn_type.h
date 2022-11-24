#pragma once

#include <QSerialPort>

#include "base_conn_type.h"
#include "log.h"

class COM_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    COM_conn_type(const QString& name, QObject *parent);

    struct SetingsPort
    {
        qint32 baudRate;
        int dataBits;
        int parity;
        int stopBits;
        int flowControl;
    };

    void setSettingsPort(const SetingsPort &settingsPort);

    virtual qint64 write(const QByteArray &data) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;

protected:
    virtual QByteArray readAll() override;

private:
    QSerialPort _serialPort;

private slots:
    void slotHandleError(QSerialPort::SerialPortError error);
};
