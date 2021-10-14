#include <QCoreApplication>

#include "iot_server.h"
#include "network_module.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Network_Module netServer;

    netServer.listen(QHostAddress("127.0.0.1"), 2021);
    Log::write("Start service on 127.0.0.1:2021");

    return a.exec();
}
