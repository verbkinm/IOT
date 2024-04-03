#include <QCoreApplication>

#include "wrapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion("0.12");

    Log::write("Start program IOTV_Server " + QString(a.applicationVersion()),
               Log::Write_Flag::FILE_STDOUT,
               ServerLog::DEFAULT_LOG_FILENAME);

//    QDir dir;
//    dir.mkpath(QFileInfo({ServerLog::HOST_LOG_FOLDER}, "test").absoluteFilePath());
//    dir.setPath(QFileInfo({ServerLog::HOST_LOG_FOLDER}, "test").absoluteFilePath());
//    qDebug() << QFileInfo({ServerLog::HOST_LOG_FOLDER}, "test").absoluteFilePath();
//    qDebug() << dir.absolutePath();

    Wrapper wrapper;

    return a.exec();
}
