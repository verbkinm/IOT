#include "file_conn_type.h"

File_conn_type::File_conn_type(const QString &name, const QString& fileName, QObject *parent) : Base_conn_type(name, parent), _file(fileName)
{
    _type = Conn_type::FILE;
//    connect(&_reconnectTimer, &QTimer::timeout, this, &File_conn_type::connectToHost);
}

qint64 File_conn_type::write(const QByteArray &data, qint64 size)
{
    Q_UNUSED(data);
    Q_UNUSED(size);
    return -1;
//    Log::write(_name + ": data transmit to " + _file.fileName() + " -> " + data.toHex(':'), Log::Write_Flag::FILE_STDOUT);

//    if((data.size() == 1) && (data[0] == IOTV_SH::QUERY_WAY_BYTE))
//    {
//        QByteArray recv;
//        recv.push_back(IOTV_SH::RESPONSE_WAY_BYTE);
//        recv.push_back(3);
//        recv.push_back(_name.size() >> 8);
//        recv.push_back(_name.size());
//        recv.push_back(0x11);
//        recv.append(QByteArray(_name.toStdString().c_str()));
//        recv.push_back(static_cast<char>(Raw::DATA_TYPE::STRING));
//        recv.push_back(static_cast<char>(Raw::DATA_TYPE::STRING));

//        _host_buffer_data = recv;
//        slotReadData();

//        return 0;
//    }
//    else if((data.size() == 1) && (data[0] == IOTV_SH::QUERY_READ_BYTE))
//    {
//        if(QFileInfo(_file).size() > BUFFER_MAX_SIZE)
//        {
//            Log::write("File " + _file.fileName() + " is large!");
//            return 0;
//        }

//        if (!_file.open(QIODevice::ReadOnly))
//        {
//            Log::write(_name + ": can't read file" + QFileInfo(_file).absoluteFilePath());
//            return -1;
//        }

//        QByteArray fileData = _file.readAll();
//        _file.close();

//        QByteArray recv;
//        recv.push_back(IOTV_SH::RESPONSE_READ_BYTE);

//        recv.push_back(fileData.size() >> 8);
//        recv.push_back(fileData.size());
//        recv.append(fileData);

//        _host_buffer_data = recv;
//        slotReadData();
//    }
//    else if(data.size() >= 3 && data[0] == IOTV_SH::QUERY_WRITE_BYTE)
//    {
//        if(data.size() != (3 + (data.at(1) | data.at(2))) )
//        {
//            Log::write(_name + ": data transimt error" + " -> " + data.toHex(':'));
//            return -1;
//        }

//        QByteArray buffer = data.mid(3);

//        if(!_file.open(QIODevice::WriteOnly))
//        {
//            Log::write(_name + ": can't write to file " + QFileInfo(_file).absoluteFilePath());
//            return -1;
//        }
//        _file.write(buffer);
//        _file.close();

//        QByteArray recv;
//        recv.push_back(data.at(0) | IOTV_SH::RESPONSE_WRITE_BYTE);

//        _host_buffer_data = recv;
//        slotReadData();
//    }
//    else if(data.size() == 1 && data[0] == IOTV_SH::QUERY_PING_BYTE)
//    {
//        IOTV_SH::RESPONSE_PONG pkg;
//        pkg.state = true;

//        QByteArray recv;
//        recv.push_back(IOTV_SH::RESPONSE_PONG_BYTE);

//        _host_buffer_data = recv;
//        slotReadData();
//    }
//    else
//    {
//        Log::write(_name + ": error data" + Q_FUNC_INFO + " " + QFileInfo(_file).absoluteFilePath() + " " + data.toHex(':'));
//        _host_buffer_data.clear();
//    }

    return 0;
}

void File_conn_type::connectToHost()
{
//    _reconnectTimer.stop();
    if(!_file.open(QIODevice::ReadWrite))
    {
        Log::write(_name + ": can't open file to read write " + QFileInfo(_file).absoluteFilePath(),
                   Log::Write_Flag::FILE_STDOUT,
                   ServerLog::DEFAULT_LOG);
//        _reconnectTimer.start(DEFAULT_INTERVAL);
        return;
    }
    _file.close();

    Log::write(_name + ": file " + QFileInfo(_file).absoluteFilePath() + " is open.",
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG);
    emit signalConnected();
}

void File_conn_type::disconnectFromHost()
{
    Log::write(_name + ": close file " + QFileInfo(_file).absoluteFilePath(),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG);
//    _reconnectTimer.start(DEFAULT_INTERVAL);

    emit signalDisconnected();
}

QByteArray File_conn_type::readAll()
{
    return {};
}
