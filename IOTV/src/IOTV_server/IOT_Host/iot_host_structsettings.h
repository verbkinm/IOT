#pragma once

#include <QString>

struct IOT_Host_StructSettings
{
    QString name,
            connection_type,
            address,
            logFile;

    uint interval;
};

struct IOT_Host_StructSettings_TCP : IOT_Host_StructSettings
{
    IOT_Host_StructSettings_TCP(IOT_Host_StructSettings *base)
    {
        if (base == nullptr)
            return;

        this->name = base->name;
        this->connection_type = base->connection_type;
        this->address = base->address;
        this->logFile = base->logFile;
        this->interval = base->interval;
    }
    uint32_t port;
};

