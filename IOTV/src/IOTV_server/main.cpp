#include <QCoreApplication>

#include "wrapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion("0.16");

    Log::write("Start program IOTV_Server " + QString(a.applicationVersion()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

    Wrapper wrapper(&a);

    return a.exec();
}
