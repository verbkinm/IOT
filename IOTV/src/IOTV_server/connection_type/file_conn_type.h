#pragma once

#include <QFileInfo>

#include "base_conn_type.h"
#include "log.h"
#include "IOTV_SH.h"

class File_conn_type : public Base_conn_type
{
    Q_OBJECT
public:
    File_conn_type(const QString& name, const QString& fileName, Base_conn_type *parent = nullptr);

    virtual qint64 write(const QByteArray &data) override;
    virtual void connectToHost() override;
    virtual void disconnectFromHost() override;

    virtual QByteArray readAll() override;

private:
    QFile _file;
};

