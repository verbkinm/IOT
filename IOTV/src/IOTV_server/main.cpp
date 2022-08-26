#include <QCoreApplication>

#include "wrapper.h"

const char* programVersion = "0.2";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Log::write("Start program IOTV_Server " + QString(programVersion));

    Wrapper wrapper;

    return a.exec();
}
