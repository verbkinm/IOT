#include "mainwindow.h"

#include <QApplication>
#include <QLineEdit>
#include <QHBoxLayout>

#include "GUI/gui_server.h"
#include "GUI/Devices/gui_device_switch.h"
#include "GUI/Devices/gui_device_unknow.h"
#include "GUI/Devices/gui_device_creator.h"
#include "device.h"
#include "server.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.showMaximized();

    //    QWidget *w = new QWidget;

    //    QVBoxLayout *layout = new QVBoxLayout;
    //    w->setLayout(layout);
    //    w->show();

    //    QPushButton *btn1 = new QPushButton("1", w);
    //    QPushButton *btn2 = new QPushButton("2", w);

    //    layout->addWidget(btn1);
    //    layout->addWidget(btn2);

    //    QPushButton *ptr = qobject_cast<QPushButton*>(layout->itemAt(0)->widget());
    //    qDebug() << ptr->text();
    //    delete ptr;




    return a.exec();
}
