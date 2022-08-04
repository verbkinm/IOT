#include "log.h"

const QString Log::_FORMAT = "yyyy.MM.dd hh:mm:ss - ";

void Log::write(const QString& data, Write_Flags writeFlags, const QString &fileName)
{
    if(writeFlags.testFlag(Write_Flag::FILE))
        writeToFile(fileName, data);
    if(writeFlags.testFlag(Write_Flag::STDOUT))
        writeToStdOut(data);
    if(writeFlags.testFlag(Write_Flag::STDERR))
        writeToStdErr(data);
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
    out << QDateTime::currentDateTime().toString(_FORMAT) << data << '\n';
    out.flush();

    file.close();
}

void Log::writeToStdOut(const QString &data)
{
    std::cout << QDateTime::currentDateTime().toString(_FORMAT).toStdString() << data.toStdString() << '\n';;
}

void Log::writeToStdErr(const QString &data)
{
    std::cerr << QDateTime::currentDateTime().toString(_FORMAT).toStdString() << data.toStdString() << '\n';;
}
