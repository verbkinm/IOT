#ifndef COM_CONN_TYPE_H
#define COM_CONN_TYPE_H

#include "base_conn_type.h"
#include "../lib/Log/log.h"

class COM_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    COM_conn_type(const QString& name, Base_conn_type *parent = nullptr);

public:
    virtual void connectToHost() override;
};

#endif // COM_CONN_TYPE_H
