#include "gui_server.h"
#include "ui_gui_server.h"

GUI_Server::GUI_Server(const QString &name, const QString &address, const QString &imagePath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUI_Server)
{
    ui->setupUi(this);

    ui->name->setText(name);
    ui->address->setText(address);
    _imagePath = imagePath;
    ui->image->setPixmap(QPixmap(_imagePath));

    connect(ui->connect, &QPushButton::clicked, this, &GUI_Server::signalConnect);
    connect(ui->list, &QPushButton::clicked, this, &GUI_Server::signalList);
    connect(ui->edit, &QPushButton::clicked, this, &GUI_Server::slotEdit);
    connect(ui->deleteServer, &QPushButton::clicked, this, &GUI_Server::slotDelete);
}

GUI_Server::~GUI_Server()
{
    delete ui;
}

void GUI_Server::setDevices(uint online, uint offline)
{
    ui->devices->setText(QString::number(online + offline) +
                         "("
                         "<fon color='green'>" + QString::number(online) + "</font>/"
                         "<fon color='red'>" + QString::number(offline) + "</font>"
                         ")");
}

void GUI_Server::setState(bool state)
{
    if(state)
    {
        ui->state->setText("<font color='green'>Connected</font>");
        ui->connect->setIcon(QIcon(":/connect"));
    }
    else
    {
        ui->state->setText("<font color='red'>Disconnected</font>");
        ui->connect->setIcon(QIcon(":/disconnect"));
    }
}

void GUI_Server::slotEdit()
{
    GUI_Edit_Server gui_edit_server(ui->name->text(), ui->address->text(), _imagePath);
    gui_edit_server.show();

    if(gui_edit_server.exec() == QDialog::Accepted)
    {
        ui->name->setText(gui_edit_server.getName());
        ui->address->setText(gui_edit_server.getAddress());
        _imagePath = gui_edit_server.getImagePath();
        ui->image->setPixmap(QPixmap(_imagePath));

        emit signalEdit();
    }
}

void GUI_Server::slotDelete()
{
    QMessageBox::StandardButton  result = QMessageBox::question(this, "Delete server", "Are you sure want delete this server?", QMessageBox::Ok | QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
        emit signalDelete();
}
