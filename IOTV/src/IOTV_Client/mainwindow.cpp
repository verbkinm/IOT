#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->addTab(&_serverTab, "Servers");
    ui->tabWidget->removeTab(0);

    connect(&_serverTab, &Tab::signalAdd, this, &MainWindow::on_actionAdd_server_triggered);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::slotTabChange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAdd_room_triggered()
{
    Tab *tab = new Tab;
    ui->tabWidget->addTab(tab, "New room");

    connect(tab, &Tab::signalAdd, this, &MainWindow::slotAddDevice);
}

void MainWindow::on_actionDelete_triggered()
{
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
}

void MainWindow::slotAddDevice()
{
//    Tab *tab = qobject_cast<Tab*>(sender());


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
    Server *server = new Server(this);
    GUI_Server *gui_server = new GUI_Server(*server);
    _servers.push_back(server);
    _gui_servers.push_back(gui_server);

    _serverTab.addWidget(gui_server);
}

void MainWindow::on_actionRemove_server_triggered()
{

}
