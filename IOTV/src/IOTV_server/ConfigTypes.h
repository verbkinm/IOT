#pragma once

struct hostField
{
    static const char* name;
    static const char* connection_type;
    static const char* address;
    static const char* interval;
    static const char* logFile;
    static const char* port;
};

struct connectionType
{
    static const char* TCP;
    static const char* FILE;
    static const char* COM;
    static const char* UDP;
};
