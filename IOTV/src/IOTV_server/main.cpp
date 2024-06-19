#include <QCoreApplication>
#include "config.h"

#include "wrapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion(QString("%1.%2").arg(ROGRAM_VERSION_MAJOR).arg(ROGRAM_VERSION_MINOR));

    Log::write(CATEGORY::SERVER, "Start program IOTV_Server " + QString(a.applicationVersion()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    Wrapper wrapper(&a);

    return a.exec();
}
