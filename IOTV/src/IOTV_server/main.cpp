#include <QCoreApplication>

#include "wrapper.h"

const char* programVersion = "0.3";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Log::write("Start program IOTV_Server " + QString(programVersion),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG);

    Wrapper wrapper;

    return a.exec();
}
