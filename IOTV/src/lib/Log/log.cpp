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
        std::cerr << "Error write to file " << fileName.toStdString() << '\n';
        return;
    }

    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss - ") << data << '\n';
    out.flush();

    file.close();
}

void Log::writeToStdOut(const QString &data)
{
    std::cout << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss - ").toStdString() << data.toStdString() << '\n';;
}

void Log::writeToStdErr(const QString &data)
{
    std::cerr << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss - ").toStdString() << data.toStdString() << '\n';;
}
