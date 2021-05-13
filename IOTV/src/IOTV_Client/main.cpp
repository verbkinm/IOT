#include "mainwindow.h"

#include <QApplication>
#include <QLineEdit>
#include <QHBoxLayout>

#include "GUI/gui_server.h"
#include "device.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
    GUI_Server server("Server #1", "localhost", "image");
    server.show();

    Device d("name");

    return a.exec();
}
