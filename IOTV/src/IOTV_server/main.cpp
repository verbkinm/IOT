#include <QCoreApplication>

#include "wrapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Wrapper wrapper;

    QByteArray arr;

    return a.exec();
}
