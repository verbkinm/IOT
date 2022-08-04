#include "file_conn_type.h"

File_conn_type::File_conn_type(const QString &name, const QString& fileName, Base_conn_type *parent) : Base_conn_type(name, parent), _file(fileName)
{
    _type = Conn_type::FILE;
    connect(&_reconnectTimer, &QTimer::timeout, this, &File_conn_type::connectToHost);
}

qint64 File_conn_type::write(const QByteArray &data)
{
    Log::write(_name + ": data transmit to " + _file.fileName() + " -> " + data.toHex(':'));

    if(data.size() == 1 && data[0] == IOTV_SH::QUERY_WAY_BYTE)
    {
        IOTV_SH::RESPONSE_WAY pkg;
        pkg.id = 3;
        pkg.description = _file.fileName().toStdString();
        pkg.readChannel.push_back(Raw::DATA_TYPE::STRING);
        pkg.writeChannel.push_back(Raw::DATA_TYPE::STRING);

        emit signalDataRiceved(pkg);

        return 0;
    }
    else if(data.size() == 1 && data[0] == IOTV_SH::QUERY_READ_BYTE)
    {
        if(QFileInfo(_file).size() > BUFFER_MAX_SIZE)
        {
            Log::write("File " + _file.fileName() + " is large!");
            return 0;
        }

        _file.close();
        _file.open(QIODevice::ReadOnly);

        IOTV_SH::RESPONSE_READ pkg;
        pkg.data = _file.readAll();

        emit signalDataRiceved(pkg);
    }
    else if(data.size() >= 3 && data[0] == IOTV_SH::QUERY_WRITE_BYTE)
    {
        if(data.size() != (3 + (data.at(1) | data.at(2))) )
        {
            Log::write(_name + ": data transimt error" + " -> " + data.toHex(':'));
            return 0;
        }

        IOTV_SH::RESPONSE_WRITE pkg;
        pkg.chanelNumber = 0;

        QByteArray buffer = data.mid(3);

        _file.close();
        if(!_file.open(QIODevice::WriteOnly))
        {
            Log::write(_name + ": can't write to file " + QFileInfo(_file).absoluteFilePath());
            return 0;
        }
        _file.write(buffer);
        _file.close();

        _file.open(QIODevice::ReadOnly);

        Log::write(_name + ": data riceved from  " + _file.fileName() + " <- " + buffer.toHex(':'));
        emit signalDataRiceved(pkg);
    }
    else if(data.size() == 1 && data[0] == IOTV_SH::QUERY_PING_BYTE)
    {
        IOTV_SH::RESPONSE_PONG pkg;
        pkg.state = true;

        Log::write(_name + ": data riceved from  " + _file.fileName() + " <- " + data.toHex(':'));

        emit signalDataRiceved(pkg);
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
