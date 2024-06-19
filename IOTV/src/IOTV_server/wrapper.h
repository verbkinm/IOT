#pragma once

#include <QFileSystemWatcher>

#include "IOTV_Server/iotv_server.h"

class Wrapper : public QObject
{
    Q_OBJECT
public:
    explicit Wrapper(QObject *parent = nullptr);

private:
    QFileSystemWatcher _watcher;
    std::unique_ptr<IOTV_Server> _server;

private slots:
    void slotFileChanged(const QString &fileName);
    void slotServerDestroy();
};
