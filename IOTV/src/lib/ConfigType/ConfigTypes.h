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

struct ServerLog
{
    static QString TCP_LOG_FILENAME ;
    static QString CLIENT_ONLINE_LOG_FILENAME;
    static QString DEFAULT_LOG_FILENAME;

    static const QString TCP_LOG;
    static const QString CLIENT_ONLINE_LOG;
    static const QString DEFAULT_LOG;
};
