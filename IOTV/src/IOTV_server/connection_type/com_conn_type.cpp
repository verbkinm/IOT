#include "com_conn_type.h"

COM_conn_type::COM_conn_type(const QString& name, Base_conn_type *parent) : Base_conn_type(name, parent)
{
    _type = Conn_type::COM;

    connect(&_serialPort, &QSerialPort::errorOccurred, this, &COM_conn_type::slotHandleError);
    connect(&_serialPort, &QSerialPort::readyRead, this, &COM_conn_type::slotReadData);
    connect(&_reconnectTimer, &QTimer::timeout, this, &COM_conn_type::connectToHost);
}

void COM_conn_type::connectToHost()
{
    if (!_serialPort.open(QIODevice::ReadWrite))
    {
        Log::write(_name + " Error open port");
        _reconnectTimer.start(DEFAULT_INTERVAL);
        return;
    }
    _reconnectTimer.stop();
    Log::write(_name + ": connected to " + _address);
    emit signalConnected();
}

void COM_conn_type::setSettingsPort(const SetingsPort &settingsPort)
{
    _serialPort.setPortName(_address);

    switch(settingsPort.baudRate)
    {
    case 1200:
        _serialPort.setBaudRate(QSerialPort::Baud1200);
        break;
    case 2400:
        _serialPort.setBaudRate(QSerialPort::Baud2400);
        break;
    case 4800:
        _serialPort.setBaudRate(QSerialPort::Baud4800);
        break;
    case 9600:
        _serialPort.setBaudRate(QSerialPort::Baud9600);
        break;
    case 19200:
        _serialPort.setBaudRate(QSerialPort::Baud19200);
        break;
    case 38400:
        _serialPort.setBaudRate(QSerialPort::Baud38400);
        break;
    case 115200:
        _serialPort.setBaudRate(QSerialPort::Baud115200);
        break;
    default:
        _serialPort.setBaudRate(QSerialPort::Baud9600);
        break;
    }

    switch(settingsPort.dataBits)
    {
    case 5:
        _serialPort.setDataBits(QSerialPort::Data5);
        break;
    case 6:
        _serialPort.setDataBits(QSerialPort::Data6);
        break;
    case 7:
        _serialPort.setDataBits(QSerialPort::Data7);
        break;
    case 8:
        _serialPort.setDataBits(QSerialPort::Data8);
        break;
    default:
        _serialPort.setDataBits(QSerialPort::Data8);
        break;
    }

    switch (settingsPort.parity)
    {
    case 0:
        _serialPort.setParity(QSerialPort::NoParity);
        break;
    case 1:
        _serialPort.setParity(QSerialPort::EvenParity);
        break;
    case 2:
        _serialPort.setParity(QSerialPort::OddParity);
        break;
    case 3:
        _serialPort.setParity(QSerialPort::MarkParity);
        break;
    case 4:
        _serialPort.setParity(QSerialPort::SpaceParity);
        break;
    default:
        _serialPort.setParity(QSerialPort::NoParity);
        break;
    }

    switch (settingsPort.stopBits)
    {
    case 1:
        _serialPort.setStopBits(QSerialPort::OneStop);
        break;
    case 2:
        _serialPort.setStopBits(QSerialPort::TwoStop);
        break;
    default:
        _serialPort.setStopBits(QSerialPort::OneStop);
        break;
    }

    switch (settingsPort.flowControl)
    {
    case 0:
        _serialPort.setFlowControl(QSerialPort::NoFlowControl);
        break;
    case 1:
        _serialPort.setFlowControl(QSerialPort::HardwareControl);
        break;
    case 2:
        _serialPort.setFlowControl(QSerialPort::SoftwareControl);
        break;
    default:
        _serialPort.setFlowControl(QSerialPort::NoFlowControl);
        break;
    }
}

void COM_conn_type::slotHandleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError || error == QSerialPort::ReadError)
        Log::write(_name + " Error port");

    disconnectFromHost();
}


qint64 COM_conn_type::write(const QByteArray &data)
{
    Log::write(_name + ": data transmit to " + _address + " -> " + data.toHex(':'));
    return _serialPort.write(data);
}

void COM_conn_type::disconnectFromHost()
{
    if(_serialPort.isOpen())
        _serialPort.close();

    emit signalDisconnected();
    _reconnectTimer.start(DEFAULT_INTERVAL);
}

QByteArray COM_conn_type::readAll()
{
    return _serialPort.readAll();
}
