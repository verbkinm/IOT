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

struct serverField
{
    static const QString address;
    static const QString portClients;
    static const QString portHosts;
    static const QString broadCastListenerPort;
    static const QString maxClient;
};

struct ServerLog
{
    static QString TCP_LOG_FILENAME;
    static QString CLIENT_ONLINE_LOG_FILENAME;
    static QString DEFAULT_LOG_FILENAME;

    static const QString TCP_LOG;
    static const QString CLIENT_ONLINE_LOG;
    static const QString DEFAULT_LOG;
};

struct Json_Event_Action
{
    static const QString EVENT_ACTION_FILE_NAME;

    static const QString EVENT_ACTION_NAME;
    static const QString EVENT;
    static const QString ACTION;

    static const QString HOST_NAME;
    static const QString HOST_DST;
    static const QString HOST_SRC;

    static const QString TYPE;
    static const QString TYPE_CONN;
    static const QString TYPE_DISCONN;
    static const QString TYPE_STATE;

    static const QString TYPE_DATA;
    static const QString TYPE_DATA_TX;
    static const QString TYPE_DATA_TX_REF;
    static const QString TYPE_ALARM;
    static const QString TYPE_TIMER;

    static const QString TYPE_CMD_CLIENT;
    static const QString TYPE_MESSAGE;

    static const QString STATE;
    static const QString STATE_ONLINE;
    static const QString STATE_OFFNLINE;
    static const QString STATE_SWITCH;

    static const QString DIRECTION;
    static const QString DIRECTION_TX;
    static const QString DIRECTION_RX;
    static const QString DIRECTION_ANY;
    static const QString DIRECTION_CHANGE;

    static const QString COMPARE;
    static const QString COMPARE_EQ;
    static const QString COMPARE_NE;
    static const QString COMPARE_GR;
    static const QString COMPARE_LS;
    static const QString COMPARE_GE;
    static const QString COMPARE_LE;
    static const QString COMPARE_ALWAYS_TRUE;
    static const QString COMPARE_ALWAYS_FALSE;

    static const QString CH_NUM;
    static const QString CH_NUM_DST;
    static const QString CH_NUM_SRC;

    static const QString DATA;
    static const QString DATA_TYPE;
    static const QString DATA_TYPE_INT_8;
    static const QString DATA_TYPE_INT_16;
    static const QString DATA_TYPE_INT_32;
    static const QString DATA_TYPE_INT_64;
    static const QString DATA_TYPE_FLOAT_32;
    static const QString DATA_TYPE_DOUBLE_64;
    static const QString DATA_TYPE_BOOL;
    static const QString DATA_TYPE_STRING;
    static const QString DATA_TYPE_RAW;
    static const QString DATA_TYPE_NONE;

    static const QString ALARM_TIME;
    static const QString ALARM_DAYS;

    static const QString TIMER_SECONDS;
};
