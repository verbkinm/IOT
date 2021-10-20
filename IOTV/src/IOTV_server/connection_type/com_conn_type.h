#ifndef COM_CONN_TYPE_H
#define COM_CONN_TYPE_H

#include <QSerialPort>

#include "base_conn_type.h"
#include "../lib/Log/log.h"


class COM_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    COM_conn_type(const QString& name, Base_conn_type *parent = nullptr);

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

private:
    QSerialPort _serialPort;

private slots:
    void slotReadData();
    void slotHandleError(QSerialPort::SerialPortError error);


};

#endif // COM_CONN_TYPE_H
