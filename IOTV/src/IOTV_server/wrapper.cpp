#include "wrapper.h"

Wrapper::Wrapper(QObject *parent) : QObject(parent), _server(std::make_unique<IOTV_Server>())
{
    _watcher.addPaths(_server->getFileSettingNames());
    connect(&_watcher, &QFileSystemWatcher::fileChanged, this, &Wrapper::slotFileChange);
}

void Wrapper::slotFileChange(QString fileName)
{
    _watcher.addPaths(_server->getFileSettingNames());
    Log::write("Setting file changed: " + fileName,
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG);
    _server = std::make_unique<IOTV_Server>();
}
