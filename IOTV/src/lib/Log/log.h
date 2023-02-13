#pragma once

#include <mutex>

#include <QDebug>
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QTextStream>

#include "ConfigTypes.h"

class Log
{
public:
    enum class Write_Flag : uint8_t
    {
        FILE = 0x01,
        STDOUT = 0x02,
        STDERR = 0x04,
        FILE_STDOUT = FILE | STDOUT,
        FILE_STDERR = FILE | STDERR
    };
     Q_DECLARE_FLAGS(Write_Flags, Write_Flag)

    static void write(const QString& data, Write_Flags writeFlags, const QString &fileName);

private:
    static void writeToFile(const QString &fileName, const QString &data);
    static void writeToStdOut(const QString &data);
    static void writeToStdErr(const QString &data);

    static const QString _FORMAT;
    static std::mutex _mutex;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Log::Write_Flags)
