#pragma once
#include <QString>

struct hostField
{
    static const QString name;
    static const QString connection_type;
    static const QString address;
    static const QString interval;
    static const QString logFile;
    static const QString port;
};

struct connectionType
{
    static const QString TCP;
    static const QString FILE;
    static const QString COM;
    static const QString UDP;
};
