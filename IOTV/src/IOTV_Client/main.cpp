    #include "mainwindow.h"

#include <QApplication>
#include <QLineEdit>
#include <QHBoxLayout>

#include "GUI/gui_server.h"
#include "GUI/Devices/gui_device_switch.h"
#include "GUI/Devices/gui_device_unknow.h"
#include "device.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    GUI_Server server("Server #1", "localhost", "image");
//    server.show();

    auto d = std::make_shared<Device>("name");

    GUI_Device_Unknow du(d);
    du.show();

    GUI_Device_Switch ds(d);
    ds.show();

    return a.exec();
}
