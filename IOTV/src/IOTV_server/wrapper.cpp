#include "wrapper.h"

Wrapper::Wrapper(QObject *parent) : QObject(parent), _server(std::make_unique<IOTV_Server>(this))
{
    _watcher.addPaths(_server->getFileSettingNames());
    connect(&_watcher, &QFileSystemWatcher::fileChanged, this, &Wrapper::slotFileChanged);

    connect(_server.get(), &IOTV_Server::signalReadyToDestroy, this, &Wrapper::slotServerDestroy, Qt::QueuedConnection);
}

void Wrapper::slotFileChanged(const QString &fileName)
{
    //    _watcher.addPaths(_server->getFileSettingNames());
    Log::write(CATEGORY::INFO, "Setting file changed: " + fileName,
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    emit _server->signalDestroy();
    //    _server = std::make_unique<IOTV_Server>();
}

void Wrapper::slotServerDestroy()
{
    _server = std::make_unique<IOTV_Server>();
    connect(_server.get(), &IOTV_Server::signalReadyToDestroy, this, &Wrapper::slotServerDestroy, Qt::QueuedConnection);
}
