#include "file_conn_type.h"

File_conn_type::File_conn_type(const QString &name, const QString& fileName, Base_conn_type *parent) : Base_conn_type(name, parent), _file(fileName)
{
    connect(&_reconnectTimer, &QTimer::timeout, this, &File_conn_type::connectToHost);
}


qint64 File_conn_type::write(const QByteArray &data)
{
    QString strOut = _name + ": data transmit to " + _file.fileName() + " -> " + data.toHex(':');
    Log::write(strOut);

    QByteArray buffer;
    if(data.size() == 1 && data[0] == QUERY_WAY_BYTE)
    {
        buffer.clear();
        uint8_t byte = 0x05;
        buffer.push_back(byte);

        buffer.push_back(3);

        byte = _file.fileName().size() >> 8;
        buffer.push_back(byte);
        byte = _file.fileName().size();
        buffer.push_back(byte);

        byte = 1 << 4;
        byte |= 1;
        buffer.push_back(byte);

        for (char byte : _file.fileName().toStdString())
            buffer.push_back(byte);

        buffer.push_back(Raw::toUInt8(Raw::DATA_TYPE::CHAR_PTR));
        buffer.push_back(Raw::toUInt8(Raw::DATA_TYPE::CHAR_PTR));

        strOut = _name + ": data riceved from  " + _file.fileName() + " <- " + buffer.toHex(':');
        Log::write(strOut);

        emit signalDataRiceved(buffer);
        return 0;
    }
    else if(data.size() == 1 && data[0] == QUERY_READ_BYTE)
    {
        _file.reset();
        Log::write(_file.readAll(), Log::Flags::WRITE_TO_STDOUT_ONLY);
    }

    return 0;
}

void File_conn_type::connectToHost()
{
    if(!_file.open(QIODevice::ReadOnly))
    {
        QString strOut = _name + ": can't open file " + QFileInfo(_file).absoluteFilePath();
        Log::write(strOut);
        _reconnectTimer.start(DEFAULT_INTERVAL);
        return;
    }

    QString strOut = _name + ": file " + QFileInfo(_file).absoluteFilePath() + " is open.";
    Log::write(strOut);
    _reconnectTimer.stop();

    emit signalConnected();
}

void File_conn_type::disconnectFromHost()
{
    QString strOut = _name + ": close file " + QFileInfo(_file).absoluteFilePath();
    Log::write(strOut);

    emit signalDisconnected();

    _file.close();
}
