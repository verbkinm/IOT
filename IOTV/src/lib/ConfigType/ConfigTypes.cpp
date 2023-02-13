#include "ConfigTypes.h"

const QString hostField::name = "name";
const QString hostField::connection_type = "connection_type";
const QString hostField::address = "address";
const QString hostField::interval = "interval";
const QString hostField::logFile = "log_file";
const QString hostField::port = "port";

const QString connectionType::TCP = "TCP";
const QString connectionType::FILE = "FILE";
const QString connectionType::COM = "COM";
const QString connectionType::UDP = "UDP";

QString ServerLog::TCP_LOG = "server.log";
QString ServerLog::CLIENT_ONLINE_LOG = "client_online.log";
QString ServerLog::DEFAULT_LOG = "default.log";
