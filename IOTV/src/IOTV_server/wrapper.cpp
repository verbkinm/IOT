#include "wrapper.h"

Wrapper::Wrapper(QObject *parent) : QObject(parent), _server(std::make_unique<IOT_Server>())
{
    Log::write("Start program IOTV_Srver " + _server->getProgramVersion());

    _watcher.addPath(_server->getFileSettingName());
    connect(&_watcher, SIGNAL(fileChanged(QString)), SLOT(slotFileChange()) );
}

void Wrapper::slotFileChange()
{
    Log::write("Setting file changed!");
    _server = std::make_unique<IOT_Server>();
}
