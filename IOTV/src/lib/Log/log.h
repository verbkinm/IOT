#pragma once

#include <QString>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <iostream>

#include "Log_global.h"

#define LOG_FILE_NAME "IOTV.log"

class LOG_EXPORT Log
{
//    Q_OBJECT
public:

    enum class Write_Flag
    {
        FILE = 0x01,
        STDOUT = 0x02,
        STDERR = 0x04,
        FILE_STDOUT = FILE | STDOUT,
        FILE_STDERR = FILE | STDERR
    };
     Q_DECLARE_FLAGS(Write_Flags, Write_Flag)
//     Q_FLAG(Write_Flags)

    static void write(const QString& data, Write_Flags writeFlags = Write_Flag::STDOUT, const QString &fileName = LOG_FILE_NAME);

private:
    static void writeToFile(const QString &fileName, const QString &data);
    static void writeToStdOut(const QString &data);
    static void writeToStdErr(const QString &data);

    static const QString _format;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Log::Write_Flags)
