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

const QString serverField::address = "address";
const QString serverField::port = "port";
const QString serverField::maxClient = "max_client";

QString ServerLog::TCP_LOG_FILENAME = "tcp_server.log";
QString ServerLog::CLIENT_ONLINE_LOG_FILENAME = "client_online.log";
QString ServerLog::DEFAULT_LOG_FILENAME = "default.log";

const QString ServerLog::TCP_LOG = "tcp_log";
const QString ServerLog::CLIENT_ONLINE_LOG = "client_log";
const QString ServerLog::DEFAULT_LOG = "default_log";


//Json Event and Action

const QString Json_Event_Action::EVENT = "event";
const QString Json_Event_Action::ACTION = "action";

const QString Json_Event_Action::HOST_NAME = "host_name";
const QString Json_Event_Action::HOST_DST = "dst_host";
const QString Json_Event_Action::HOST_SRC = "src_host";

const QString Json_Event_Action::TYPE = "type";
const QString Json_Event_Action::TYPE_CONN = "connection";
const QString Json_Event_Action::TYPE_DISCONN = "disconnection";
const QString Json_Event_Action::TYPE_STATE = "state";
const QString Json_Event_Action::TYPE_DATA = "data";
const QString Json_Event_Action::TYPE_DATA_TX = "data_tx";
const QString Json_Event_Action::TYPE_DATA_RX = "data_rx";
const QString Json_Event_Action::TYPE_DATA_TX_REF = "data_tx_ref";
const QString Json_Event_Action::TYPE_DATA_RX_REF = "data_rx_ref";
const QString Json_Event_Action::TYPE_CMD_CLIENT = "cmd_client";
const QString Json_Event_Action::TYPE_MESSAGE = "message";

const QString Json_Event_Action::STATE = "state";
const QString Json_Event_Action::STATE_ONLINE = "online";
const QString Json_Event_Action::STATE_OFFNLINE = "offline";
const QString Json_Event_Action::STATE_SWITCH = "switch";

const QString Json_Event_Action::DIRECTION = "direction";
const QString Json_Event_Action::DIRECTION_TX = "tx";
const QString Json_Event_Action::DIRECTION_RX = "rx";
const QString Json_Event_Action::DIRECTION_ANY = "any";

const QString Json_Event_Action::COMPARE = "compare";
const QString Json_Event_Action::COMPARE_EQ = "==";
const QString Json_Event_Action::COMPARE_NE = "!=";
const QString Json_Event_Action::COMPARE_GR = ">";
const QString Json_Event_Action::COMPARE_LS = "<";
const QString Json_Event_Action::COMPARE_GE = ">=";
const QString Json_Event_Action::COMPARE_LE = "<=";

const QString Json_Event_Action::CH_NUM = "chNumber";
const QString Json_Event_Action::CH_NUM_DST = "dst_chNumber";
const QString Json_Event_Action::CH_NUM_SRC = "src_chNumber";

const QString Json_Event_Action::DATA = "data";

