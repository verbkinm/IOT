#pragma once

#include <QString>

struct IOT_Host_StructSettings
{
    QString name,
            connection_type,
            address,
            logFile;

    uint port,
         interval;
};

