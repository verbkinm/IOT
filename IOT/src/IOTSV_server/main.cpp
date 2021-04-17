#include <QCoreApplication>
#include <QDebug>

#include <iostream>

#include "IOT_Host/IOT_Host.h"
#include "connection_type/ethernet_conn_type.h"
#include "connection_type/com_conn_type.h"
//#include "../lib/raw/raw.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    IOT_Host host("New name");
    host.setConnectionTypeEthernet("127.0.0.1", 2021);
    host.connectToHost();
    host.printDebugData();

    return a.exec();
}
