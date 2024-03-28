#include "log.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTextStream>

const QString Log::_FORMAT = "yyyy.MM.dd hh:mm:ss:zzz - ";
std::mutex Log::_mutex;

const int MAX_STRING_SIZE = 200;

void Log::write(const QString& data, Write_Flags writeFlags, const QString &fileName)
{
    if (writeFlags.testFlag(Write_Flag::FILE))
        writeToFile(fileName, data);
    if (writeFlags.testFlag(Write_Flag::STDOUT))
        writeToStdOut(data);
    if (writeFlags.testFlag(Write_Flag::STDERR))
        writeToStdErr(data);
}

void Log::writeToFile(const QString &fileName, const QString &data)
{
    std::lock_guard lg(_mutex);
    QFile file(fileName);

    if (!file.exists())
    {
        QDir dir;
        dir.mkdir(QFileInfo(file).absolutePath());
    }

    if (!file.open(QFile::Append | QFile::Text))
    {
        qWarning() << "Error write to file " << fileName.toStdString().c_str();
        return;
    }

    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString(_FORMAT)
        << ((data.size() > MAX_STRING_SIZE) ? data.mid(0, MAX_STRING_SIZE) : data)
        << '\n';
//    out.flush();

    file.close();
}

void Log::writeToStdOut(const QString &data)
{
    std::lock_guard lg(_mutex);
    qDebug() << QDateTime::currentDateTime().toString(_FORMAT).toStdString().c_str()
             << ((data.size() > MAX_STRING_SIZE) ? data.mid(0, MAX_STRING_SIZE).toStdString().c_str() : data.toStdString().c_str());
}

void Log::writeToStdErr(const QString &data)
{
    std::lock_guard lg(_mutex);
    qWarning() << QDateTime::currentDateTime().toString(_FORMAT).toStdString().c_str()
               << ((data.size() > MAX_STRING_SIZE) ? data.mid(0, MAX_STRING_SIZE).toStdString().c_str() : data.toStdString().c_str());
}

void Log::checkPath(const QString &fileName)
{

}
