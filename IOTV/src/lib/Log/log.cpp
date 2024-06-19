#include "log.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTextStream>

const QString Log::_FORMAT = "yyyy.MM.dd hh:mm:ss:zzz - ";
std::mutex Log::_mutex;

const int MAX_STRING_SIZE = 256;

void Log::write(const QString &category, const QString& data, Write_Flags writeFlags, const QString &fileName)
{
    QString categ;
    if (category != "")
        categ = "(" + category + "): ";
    else
        categ = category;

    if (writeFlags.testFlag(Write_Flag::FILE))
        writeToFile(fileName, categ, data);
    if (writeFlags.testFlag(Write_Flag::STDOUT))
        writeToStdOut(categ, data);
    if (writeFlags.testFlag(Write_Flag::STDERR))
        writeToStdErr(categ, data);
}

void Log::writeToFile(const QString &fileName, const QString &category, const QString &data)
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
        << category
        << ((data.size() > MAX_STRING_SIZE) ? data.mid(0, MAX_STRING_SIZE) : data)
        << '\n';

    file.close();
}

void Log::writeToStdOut(const QString &category, const QString &data)
{
    std::lock_guard lg(_mutex);
    QTextStream out(stdout);
    out << QDateTime::currentDateTime().toString(_FORMAT).toStdString().c_str()
        << category
        << ((data.size() > MAX_STRING_SIZE) ? data.mid(0, MAX_STRING_SIZE).toStdString().c_str() : data.toStdString().c_str())
        << '\n';
}

void Log::writeToStdErr(const QString &category, const QString &data)
{
    std::lock_guard lg(_mutex);
    QTextStream out(stderr);
    out << QDateTime::currentDateTime().toString(_FORMAT).toStdString().c_str()
        << category
        << ((data.size() > MAX_STRING_SIZE) ? data.mid(0, MAX_STRING_SIZE).toStdString().c_str() : data.toStdString().c_str())
        << '\n';
}
