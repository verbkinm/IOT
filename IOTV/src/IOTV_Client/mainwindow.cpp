#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _serverTab.setParent(ui->tabWidget);
    ui->tabWidget->addTab(&_serverTab, "Servers");
    ui->tabWidget->removeTab(0);

    connect(&_serverTab, &Tab::signalAdd, this, &MainWindow::on_actionAdd_server_triggered);
    connect(&_serverTab, &Tab::signalRemove, this, &MainWindow::on_actionRemove_server_triggered);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::slotTabChange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotAddDeviceToRoom()
{
    QObjectList result;
    for(auto &elem : _serverTab.childrenPointerList())
    {
        GUI_Server* gui_server = qobject_cast<GUI_Server*>(elem);
        for(auto const &item : gui_server->getDevices())
            result << item.second.get();
    }

    ObjectList objList(result);

    if(objList.exec() == QDialog::Accepted)
    {
        Tab_Room* tab_room = qobject_cast<Tab_Room*>(sender());
        for(auto &elem : objList.checkedObject())
            tab_room->addDevice(elem->parent()->objectName(), qobject_cast<Device*>(elem)->getName());

        tab_room->restructWidget();
    }
}

void MainWindow::slotRemoveDeviceFromRoom()
{
    Tab_Room *tab = qobject_cast<Tab_Room*>(ui->tabWidget->currentWidget());
    tab->deleteDevices();
}

void MainWindow::on_actionAdd_room_triggered()
{
    Tab_Room *room = new Tab_Room(_serverTab, this);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(room, "New room"));

    connect(room, &Tab::signalAdd, this, &MainWindow::slotAddDeviceToRoom);
    connect(room, &Tab::signalRemove, this, &MainWindow::slotRemoveDeviceFromRoom);
}

void MainWindow::on_actionDelete_triggered()
{
    int currentIndex = ui->tabWidget->currentIndex();
    delete ui->tabWidget->widget(currentIndex);
}

void MainWindow::slotTabChange(int index)
{
    if(index == -1)
        return;

    if(!ui)
        return;

    if(ui->tabWidget->tabText(index) == "Servers" )
        ui->actionDelete->setEnabled(false);
    else
        ui->actionDelete->setEnabled(true);
}

void MainWindow::on_actionAdd_server_triggered()
{
    Server *server = new Server;
    _serverList.push_back(server);

    GUI_Server *gui_server = new GUI_Server(*server, _serverTab);

    _serverTab.addWidget(gui_server);

    ui->actionRemove_server->setEnabled(true);

    connect(server, &Server::signalDeviceCreated, this, &MainWindow::roomsRestructWidget);
}

void MainWindow::on_actionRemove_server_triggered()
{
    ObjectList objList(_serverTab.childrenPointerList());

    if(objList.exec() == QDialog::Accepted)
    {
        for(auto &elem : objList.checkedObject())
        {
            for (auto &obj : _serverList)
            {
                if(elem->objectName() == obj->objectName())
                {
                    delete obj;
                    _serverList.remove(obj);
                    break;
                }
            }
        }
        if(_serverTab.childrenPointerList().length() == 0)
            ui->actionRemove_server->setEnabled(false);
    }
}

void MainWindow::roomsRestructWidget()
{
    for(int i = 1; i < ui->tabWidget->count(); i++)
        qobject_cast<Tab_Room*>(ui->tabWidget->widget(i))->restructWidget();
}


void MainWindow::closeEvent(QCloseEvent *)
{
    qApp->exit();
}
