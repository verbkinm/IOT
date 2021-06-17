#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QFile>
#include <QDateTime>
#include <QDebug>

#include "Log_global.h"

#define LOG_FILE_NAME "IOTV.log"

class LOG_EXPORT Log
{
public:

    enum class Flags
    {
        WRITE_TO_FILE_ONLY,
        WRITE_TO_STDOUT_ONLY,
        WRITE_TO_STDERR_ONLY,
        WRITE_TO_FILE_AND_STDOUT,
        WRITE_TO_FILE_AND_STDERR
    };

    static void write(const QString& data, Flags pathToWrite = Log::Flags::WRITE_TO_FILE_AND_STDOUT, const QString &fileName = LOG_FILE_NAME);

private:
    static void writeToFile(const QString &fileName, const QString &data);
    static void writeToStdOut(const QString &data);
    static void writeToStdErr(const QString &data);
};

#endif // LOG_H
