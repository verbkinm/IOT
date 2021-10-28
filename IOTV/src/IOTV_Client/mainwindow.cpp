#include "mainwindow.h"
#include "ui_mainwindow.h"

QString server_tab_text = "Servers";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _settings(QSettings::IniFormat, QSettings::UserScope, "VMS", "IOTV_Client")
{
    ui->setupUi(this);

    _serverTab.setParent(ui->tabWidget);
    ui->tabWidget->addTab(&_serverTab, server_tab_text);
    ui->tabWidget->removeTab(0);

    connect(&_serverTab, &Tab::signalAdd, this, &MainWindow::on_actionAdd_server_triggered);
    connect(&_serverTab, &Tab::signalRemove, this, &MainWindow::on_actionRemove_server_triggered);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::slotTabChange);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::slotCloseTab);
    connect(ui->tabWidget, &QTabWidget::tabBarDoubleClicked, this, &MainWindow::slotRenameTab);

    checkSettingsFileExist();
    readSettings();
}

MainWindow::~MainWindow()
{
    disconnect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::slotTabChange);
    disconnect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::slotCloseTab);

    for (auto &elem : _serverList)
    {
        if(elem != nullptr)
            delete elem;
    }

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
    static uint roomNumber = 0;
    Tab_Room *room = new Tab_Room(_serverTab, this);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(room, "New room " + QString::number(roomNumber++)));

    connect(room, &Tab::signalAdd, this, &MainWindow::slotAddDeviceToRoom);
    connect(room, &Tab::signalRemove, this, &MainWindow::slotRemoveDeviceFromRoom);
}

void MainWindow::on_actionDelete_room_triggered()
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

    if(ui->tabWidget->tabText(index) == server_tab_text )
        ui->actionDelete_room->setEnabled(false);
    else
        ui->actionDelete_room->setEnabled(true);
}

void MainWindow::slotCloseTab(int index)
{
    if(ui->tabWidget->tabText(index) == server_tab_text)
        return;

    if(QMessageBox::question(this, "Delete tab", "Are you reale delete this tab?") == QMessageBox::Yes)
        delete ui->tabWidget->widget(index);
}

void MainWindow::slotRenameTab(int index)
{
    if(ui->tabWidget->tabText(index) == server_tab_text)
        return;

    bool ok = false;
    QString text = QInputDialog::getText(this, "New name",
                                         tr("Input new name for tab:"), QLineEdit::Normal,
                                         ui->tabWidget->tabText(index), &ok);
    if (ok && !text.isEmpty())
        ui->tabWidget->setTabText(index, text);
}

void MainWindow::on_actionAdd_server_triggered()
{
    Server *server = new Server;
    _serverList.push_back(server);

    GUI_Server *gui_server = new GUI_Server(*server, _serverTab);
    gui_server->update();

    _serverTab.addWidget(gui_server);

    ui->actionRemove_server->setEnabled(true);

    connect(server, SIGNAL(signalDeviceCreated()), this, SLOT(slotRoomsRestructWidget()));
    connect(server, SIGNAL(signalDisconnected()), this, SLOT(slotRoomsRestructWidget()));
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

    slotRoomsRestructWidget();
}

void MainWindow::slotRoomsRestructWidget()
{
    for(int i = 1; i < ui->tabWidget->count(); i++)
        qobject_cast<Tab_Room*>(ui->tabWidget->widget(i))->restructWidget();
}

void MainWindow::checkSettingsFileExist()
{
    if(!QFileInfo::exists(_settings.fileName()))
    {
        _settings.beginGroup("Servers");
        _settings.beginGroup("server0");
        _settings.setValue("address", "127.0.0.1");
        _settings.setValue("port", 2022);
        _settings.endGroup();
        _settings.endGroup();
        _settings.sync();
    }
}

void MainWindow::readSettings()
{
    _settings.beginGroup("Servers");
    foreach(const auto &elem, _settings.childGroups())
    {
        _settings.beginGroup(elem);

        Server *server = new Server;
        _serverList.push_back(server);

        server->setName(_settings.group().split('/').at(1));
        server->setServerAddres(_settings.value("address").toString());
        server->setServerPort(_settings.value("port").toUInt());

        _settings.beginGroup("alias");
        foreach(const auto &name, _settings.allKeys())
            server->addAlias(name, _settings.value(name).toString());
        _settings.endGroup();

        GUI_Server *gui_server = new GUI_Server(*server, _serverTab);
        gui_server->update();

        _serverTab.addWidget(gui_server);

        ui->actionRemove_server->setEnabled(true);

        connect(server, &Server::signalDevicesCreated, this, &MainWindow::slotRoomsRestructWidget);
        connect(server, &Server::signalDisconnected, this, &MainWindow::slotRoomsRestructWidget);

        _settings.endGroup();
    }
    _settings.endGroup();

    _settings.beginGroup("Rooms");
    foreach(const auto &tabNumber, _settings.childGroups())
    {
        _settings.beginGroup(tabNumber);

        Tab_Room *room = new Tab_Room(_serverTab, this);
        ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(room, _settings.value("tabText").toString()));

        connect(room, &Tab::signalAdd, this, &MainWindow::slotAddDeviceToRoom);
        connect(room, &Tab::signalRemove, this, &MainWindow::slotRemoveDeviceFromRoom);

        foreach(const auto &serverObjectName, _settings.childGroups())
        {
            _settings.beginGroup(serverObjectName);
            foreach(const auto &dev, _settings.allKeys())
                room->addDevice(serverObjectName, _settings.value(dev).toString());

            _settings.endGroup();
        }
        _settings.endGroup();
    }
    _settings.endGroup();

    slotRoomsRestructWidget();

    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::saveSettingsServers()
{
    _settings.remove("Servers");
    _settings.beginGroup("Servers");
    for (const auto &server : _serverList)
    {
        _settings.beginGroup(server->getName());
        _settings.setValue("address", server->getServerAddress());
        _settings.setValue("port", server->getServerPort());

        _settings.beginGroup("alias");
        for (const auto &[name, aliasName] : server->getAlias())
            _settings.setValue(name, aliasName);
        _settings.endGroup();

        _settings.endGroup();
    }
    _settings.endGroup();
}

void MainWindow::saveSettingsRooms()
{
    _settings.remove("Rooms");
    _settings.beginGroup("Rooms");
    QTabWidget *tabWidget = ui->tabWidget;
    for(int i = 1; i < tabWidget->count(); i++)
    {
        _settings.beginGroup(QString::number(i));
        _settings.setValue("tabText", tabWidget->tabText(i));

        Tab_Room *tabRoom = qobject_cast<Tab_Room*>(tabWidget->widget(i));
        const auto &data = tabRoom->data();
        uint devNumber = 0;
        for (const auto &[serverObjectName, deviceSet] : data)
        {
            for (const auto &deviceName : deviceSet)
            {
                _settings.beginGroup(serverObjectName);
                _settings.setValue("dev" + QString::number(devNumber++), deviceName);
                _settings.endGroup();
            }
        }
        _settings.endGroup();
    }
    _settings.endGroup();
}


void MainWindow::closeEvent(QCloseEvent *)
{
    saveSettingsServers();
    saveSettingsRooms();

    qApp->exit();
}
