#ifndef WRAPPER_H
#define WRAPPER_H

#include <QFileSystemWatcher>

#include "IOT_Server/iot_server.h"

class Wrapper : public QObject
{
    Q_OBJECT
public:
    Wrapper(QObject *parent = nullptr);

private:
    QFileSystemWatcher _watcher;
    std::unique_ptr<IOT_Server> _server;

private slots:
    void slotFileChange(QString fileName);
};

#endif // WRAPPER_H
