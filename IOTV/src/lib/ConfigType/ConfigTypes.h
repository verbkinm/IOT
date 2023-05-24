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
    static const QString port;
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
    static const QString TYPE_DATA_RX;
    static const QString TYPE_DATA_TX_REF;
    static const QString TYPE_DATA_RX_REF;

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

    static const QString COMPARE;
    static const QString COMPARE_EQ;
    static const QString COMPARE_NE;
    static const QString COMPARE_GR;
    static const QString COMPARE_LS;
    static const QString COMPARE_GE;
    static const QString COMPARE_LE;

    static const QString CH_NUM;
    static const QString CH_NUM_DST;
    static const QString CH_NUM_SRC;

    static const QString DATA;



};
