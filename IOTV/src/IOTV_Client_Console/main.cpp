#include <QCoreApplication>

#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Client client("127.0.0.1", 2022);

    client.connectToHost();

    return a.exec();
}
