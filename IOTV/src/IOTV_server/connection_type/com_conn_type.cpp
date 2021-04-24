#include "com_conn_type.h"

COM_conn_type::COM_conn_type(const QString& name, Base_conn_type *parent) : Base_conn_type(name, parent)
{
    _conn_type = Conn_type::COM;
}

void COM_conn_type::connectToHost()
{
    Log::write(QString(Q_FUNC_INFO) + " in development");
}
