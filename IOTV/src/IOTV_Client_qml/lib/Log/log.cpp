#include "log.h"

void Log::write(const QString& data, Flags pathToWrite, const QString &fileName)
{
    if(pathToWrite == Flags::WRITE_TO_STDOUT_ONLY)
        writeToStdOut(data);
    else if(pathToWrite == Flags::WRITE_TO_STDERR_ONLY)
        writeToStdErr(data);
    else if(pathToWrite == Flags::WRITE_TO_FILE_ONLY)
        writeToFile(fileName, data);
    else if(pathToWrite == Flags::WRITE_TO_FILE_AND_STDOUT)
    {
        writeToStdOut(data);
        writeToFile(fileName, data);
    }
    else if(pathToWrite == Flags::WRITE_TO_FILE_AND_STDERR)
    {
        writeToStdErr(data);
        writeToFile(fileName, data);
    }
}

void Log::writeToFile(const QString &fileName, const QString &data)
{
    QFile file(fileName);
    if(!file.open(QFile::Append | QFile::Text))
    {
        qWarning() << "Error write to file " << fileName;
        return;
    }

    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss - ") << data << Qt::endl;
    out.flush();

    file.close();
}

void Log::writeToStdOut(const QString &data)
{
    qDebug() << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss - ") << data;
}

void Log::writeToStdErr(const QString &data)
{
    qWarning() << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss - ") << data;
}
