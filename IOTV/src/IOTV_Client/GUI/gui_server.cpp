#include "gui_server.h"
#include "ui_gui_server.h"
#include "server.h"

GUI_Server::GUI_Server(Server &server,
                       const QString &imagePath,
                       QWidget *parent) :
    QFrame(parent),
    ui(new Ui::GUI_Server), _server(server)
{
    _server.attach(this);

    ui->setupUi(this);

    ui->name->setText(_server.getName());
    ui->address->setText(_server.getServerAddress() + ":" + QString::number(_server.getServerPort()));

    _imagePath = imagePath;
    ui->image->setPixmap(QPixmap(_imagePath));
    ui->image->setScaledContents(true);

    connect(ui->connect, &QPushButton::clicked, this, &GUI_Server::slotConnect);
    connect(ui->list, &QPushButton::clicked, this, &GUI_Server::slotList);
    connect(ui->edit, &QPushButton::clicked, this, &GUI_Server::slotEdit);
    connect(ui->deleteServer, &QPushButton::clicked, this, &GUI_Server::slotDelete);

    connect(this, &GUI_Server::signalDelete, &server, &Server::slotDeleteForm);
}

GUI_Server::~GUI_Server()
{
    _server.detach(this);
    delete ui;
}

void GUI_Server::setDevices(uint online, uint offline)
{
    ui->devices->setText(QString::number(online + offline) +
                         "("
                         "<font color='green'>" + QString::number(online) + "</font>/"
                                                                            "<font color='red'>" + QString::number(offline) + "</font>"
                                                                                                                              ")");
}

void GUI_Server::setState(QAbstractSocket::SocketState state)
{
    ui->connect->setEnabled(true);

    if(state == QAbstractSocket::ConnectedState)
    {
        ui->state->setText("<font color='green'>Connected</font>");
        ui->connect->setIcon(QIcon(":/connect"));
        ui->deleteServer->setDisabled(true);
        ui->edit->setDisabled(true);
    }
    else if(state == QAbstractSocket::UnconnectedState)
    {
        ui->state->setText("<font color='red'>Disconnected</font>");
        ui->connect->setIcon(QIcon(":/disconnect"));
        ui->deleteServer->setEnabled(true);
        ui->edit->setEnabled(true);
    }
}

void GUI_Server::update()
{
    ui->name->setText(_server.getName());
    ui->address->setText(_server.getServerAddress() + ":" + QString::number(_server.getServerPort()));

    uint online = _server.getDeviceOnline();
    setDevices(online, _server.getDevicesCount() - online);
    setState(_server.state());

    if(_server.deviceCount() > 0)
        ui->list->setEnabled(true);
    else
        ui->list->setDisabled(true);
}

void GUI_Server::slotEdit()
{
    GUI_Edit_Server gui_edit_server(_server.getName(),
                                    _server.getServerAddress(),
                                    QString::number(_server.getServerPort()),
                                                    _imagePath);
    gui_edit_server.show();

    if(gui_edit_server.exec() == QDialog::Accepted)
    {
        _server.setName(gui_edit_server.getName());
        _server.setServerAddres(gui_edit_server.getAddress());
        _server.setServerPort(gui_edit_server.getPort().toUInt());

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

void GUI_Server::slotConnect()
{

    if(_server.state() == QAbstractSocket::UnconnectedState)
    {
        ui->deleteServer->setDisabled(true);
        ui->edit->setDisabled(true);
        ui->state->setText("<font color='blue'>Connection attempt</font>");
        ui->connect->setDisabled(true);
        _server.connectToHost();
    }
    else /*    if(_server.state() == QAbstractSocket::ConnectedState)*/
    {
        ui->deleteServer->setEnabled(true);
        ui->edit->setEnabled(true);
        _server.disconnectFromHost();
    }
}

void GUI_Server::slotList()
{
    _server.deviceListShow();
}
