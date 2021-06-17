#include "wrapper.h"

Wrapper::Wrapper(QObject *parent) : QObject(parent), _server(std::make_unique<IOT_Server>())
{
    Log::write("Start program IOTV_Server " + _server->getProgramVersion());

    _watcher.addPaths(_server->getFileSettingNames());
    connect(&_watcher, SIGNAL(fileChanged(QString)), SLOT(slotFileChange(QString)) );
}

void Wrapper::slotFileChange(QString fileName)
{
    Log::write("Setting file changed: " + fileName);
    _server = std::make_unique<IOT_Server>();
}
