#ifndef CONNECTION_TYPE_H
#define CONNECTION_TYPE_H

#include "base_conn_type.h"

class Connection_type : public Base_conn_type
{
public:
    Connection_type(const QString& name, Base_conn_type *parent = nullptr);

    virtual qint64 write(const QByteArray &data) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;
};

#endif // CONNECTION_TYPE_H
