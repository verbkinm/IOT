#include <QCoreApplication>

#include <QThread>
#include <QtConcurrent>

#include "client.h"
#include "console_interface.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread clientThread;

    Client client("127.0.0.1", 2022);
    Console_Interface console(client);

    client.moveToThread(&clientThread);
    clientThread.start();

    QtConcurrent::run(&console, &Console_Interface::exec);

    return a.exec();
}
