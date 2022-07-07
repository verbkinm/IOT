#include "file_conn_type.h"

File_conn_type::File_conn_type(const QString &name, const QString& fileName, Base_conn_type *parent) : Base_conn_type(name, parent), _file(fileName)
{
    connect(&_reconnectTimer, &QTimer::timeout, this, &File_conn_type::connectToHost);
}


qint64 File_conn_type::write(const QByteArray &data)
{
    Log::write(_name + ": data transmit to " + _file.fileName() + " -> " + data.toHex(':'));

    QByteArray buffer;
    if(data.size() == 1 && data[0] == IOTV_SH::QUERY_WAY_BYTE)
    {
        //        buffer.clear();

        buffer.push_back(IOTV_SH::RESPONSE_WAY_BYTE);
        buffer.push_back(3); // id устройства
        buffer.push_back(_file.fileName().size() >> 8);
        buffer.push_back(_file.fileName().size());
        buffer.push_back(0x11);

        for (char byte : _file.fileName().toStdString())
            buffer.push_back(byte);

        buffer.push_back(Raw::toUInt8(Raw::DATA_TYPE::CHAR_PTR));
        buffer.push_back(Raw::toUInt8(Raw::DATA_TYPE::CHAR_PTR));

        Log::write(_name + ": data riceved from  " + _file.fileName() + " <- " + buffer.toHex(':'));

        emit signalDataRiceved(buffer);
        return 0;
    }
    else if(data.size() == 1 && data[0] == IOTV_SH::QUERY_READ_BYTE)
    {
        if(QFileInfo(_file).size() > BUFFER_MAX_SIZE)
        {
            Log::write("File " + _file.fileName() + " is large!");
            return 0;
        }

        _file.reset();
        QByteArray dataRead = _file.readAll();

        //        buffer.clear();

        buffer.push_back(IOTV_SH::RESPONSE_READ_BYTE);
        buffer.push_back(dataRead.size() >> 8);
        buffer.push_back(dataRead.size());

        for (char byte : dataRead)
            buffer.push_back(byte);

        Log::write(_name + ": data riceved from  " + _file.fileName() + " <- " + buffer.toHex(':') + " (" + dataRead + ")");

        emit signalDataRiceved(buffer);
    }
    else if(data.size() >= 3 && data[0] == IOTV_SH::QUERY_WRITE_BYTE)
    {
        if(data.size() != (3 + (data.at(1) | data.at(2))) )
        {
            Log::write(_name + ": data transimt error" + " -> " + buffer.toHex(':'));
            return 0;
        }

        buffer = data.mid(3);

        _file.close();
        if(!_file.open(QIODevice::WriteOnly))
        {
            Log::write(_name + ": can't write to file " + QFileInfo(_file).absoluteFilePath());
            return 0;
        }
        _file.write(buffer);
        _file.close();

        _file.open(QIODevice::ReadOnly);

        buffer.clear();
        buffer.push_back(IOTV_SH::RESPONSE_WRITE_BYTE);
        Log::write(_name + ": data riceved from  " + _file.fileName() + " <- " + buffer.toHex(':'));
        emit signalDataRiceved(buffer);
    }
    else if(data.size() == 1 && data[0] == IOTV_SH::QUERY_PING_BYTE)
    {
        buffer.push_back(IOTV_SH::RESPONSE_PONG_BYTE);
        Log::write(_name + ": data riceved from  " + _file.fileName() + " <- " + buffer.toHex(':'));

        emit signalDataRiceved(buffer);
    }

    return 0;
}

void File_conn_type::connectToHost()
{
    _reconnectTimer.stop();
    if(!_file.open(QIODevice::ReadOnly))
    {
        Log::write(_name + ": can't read file" + QFileInfo(_file).absoluteFilePath());
        _reconnectTimer.start(DEFAULT_INTERVAL);
        return;
    }
    Log::write(_name + ": file " + QFileInfo(_file).absoluteFilePath() + " is open.");
    emit signalConnected();
}

void File_conn_type::disconnectFromHost()
{
    Log::write(_name + ": close file " + QFileInfo(_file).absoluteFilePath());
    _reconnectTimer.start(DEFAULT_INTERVAL);

    emit signalDisconnected();

    _file.close();
}
