#include "gui_server.h"
#include "ui_gui_server.h"
#include "server.h"

GUI_Server::GUI_Server(Server &server, const Tab &serverTab, QWidget *parent) : QFrame(parent),
    ui(new Ui::GUI_Server), _imagePath(":/server"), _server(server), _serverTab(serverTab)
{
    _server.attach(this);

    ui->setupUi(this);

    ui->name->setText(_server.getName());
    ui->address->setText(_server.getServerAddress() + ":" + QString::number(_server.getServerPort()));

    ui->image->setPixmap(QPixmap(_imagePath));
    ui->image->setScaledContents(true);

    connect(ui->connect, &QPushButton::clicked, this, &GUI_Server::slotConnect);
    connect(ui->list, &QPushButton::clicked, this, &GUI_Server::slotList);
    connect(ui->edit, &QPushButton::clicked, this, &GUI_Server::slotEdit);

    //    update();
}

GUI_Server::~GUI_Server()
{
    _server.detach(this);
    delete ui;
}

const std::map<QString, std::shared_ptr<Device> > &GUI_Server::getDevices() const
{
    return _server.getDevices();
}

void GUI_Server::setDevicesState(uint online, uint offline)
{
    QString state = QString::number(online + offline) + " (<font color='green'>"
            + QString::number(online) + "</font>/" + "<font color='red'>"
            + QString::number(offline) + "</font>)";

    if(ui->devices->text() != state)
        ui->devices->setText(state);
}

void GUI_Server::setState(QAbstractSocket::SocketState state)
{
    if(!ui->connect->isEnabled())
        ui->connect->setEnabled(true);

    if(state == QAbstractSocket::ConnectedState)
    {
        QString stateText = "<font color='green'>Connected</font>";
        if(ui->state->text() != stateText)
        {
            ui->state->setText(stateText);
            ui->connect->setIcon(QIcon(":/connect"));
            ui->edit->setDisabled(true);
        }
    }
    else if(state == QAbstractSocket::UnconnectedState)
    {
        QString stateText = "<font color='red'>Disconnected</font>";
        if(ui->state->text() != stateText)
        {
            ui->state->setText(stateText);
            ui->connect->setIcon(QIcon(":/disconnect"));
            ui->edit->setEnabled(true);
        }
    }
}

QAbstractSocket::SocketState GUI_Server::state() const
{
    return _server.state();
}

void GUI_Server::update()
{
    QString objName = _server.objectName();
    if(objectName() != objName)
        setObjectName(objName);

    QString name = _server.getName();
    if(ui->name->text() != name)
        ui->name->setText(name);

    QString addr = _server.getServerAddress() + ":" + QString::number(_server.getServerPort());
    if(ui->address->text() != addr)
        ui->address->setText(addr);

    uint online = _server.getDeviceOnline();
    uint offline = _server.getDevicesCount() - online;
    setDevicesState(online, offline);

    setState(_server.state());

    if((_server.getDevicesCount() > 0) && !ui->list->isEnabled())
        ui->list->setEnabled(true);
    else if((_server.getDevicesCount() == 0) && ui->list->isEnabled())
        ui->list->setDisabled(true);
}

void GUI_Server::slotEdit()
{
    GUI_Edit_Server gui_edit_server(_server.getName(),
                                    _server.getServerAddress(),
                                    QString::number(_server.getServerPort()),
                                    _imagePath);

    if(gui_edit_server.exec() == QDialog::Accepted)
    {
        if(gui_edit_server.getName().isEmpty())
        {
            QMessageBox::warning(this, "Warning", "Server name must not be empty.", QMessageBox::Ok);
            return;
        }

        if(_server.getName() != gui_edit_server.getName())
        {
            foreach(const auto elem, _serverTab.childrenPointerList())
            {
                const GUI_Server* gui_server = qobject_cast<GUI_Server*>(elem);
                if(gui_server->ui->name->text() == gui_edit_server.getName())
                {
                    QMessageBox::warning(this, "Warning", "Server with this name alredy exist.", QMessageBox::Ok);
                    return;
                }
            }
        }
        _server.setServerNAP(gui_edit_server.getName(), gui_edit_server.getAddress(), gui_edit_server.getPort().toUInt());
        _imagePath = gui_edit_server.getImagePath();
        ui->image->setPixmap(QPixmap(_imagePath));
    }
}

void GUI_Server::slotConnect()
{

    if(_server.state() == QAbstractSocket::UnconnectedState)
    {
        ui->edit->setDisabled(true);
        ui->state->setText("<font color='blue'>Connection attempt</font>");
        ui->connect->setDisabled(true);
        _server.connectToHost();
    }
    else
    {
        ui->edit->setEnabled(true);
        _server.disconnectFromHost();
    }
}

void GUI_Server::slotList()
{
    _server.deviceListShow();
}
