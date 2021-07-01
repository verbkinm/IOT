#include "tab_room.h"

Tab_Room::Tab_Room(const Tab &serverTab, QWidget *parent) : Tab(parent), _serverTab(serverTab)
{

}

void Tab_Room::addDevice(const QString &serverName, const QString &deviceName)
{
    _data[serverName].insert(deviceName);
}

void Tab_Room::deleteDevices() //!!!
{
    ObjectList objList(childrenPointerList());

    if(objList.exec() == QDialog::Accepted)
    {
        for(auto &elem : objList.checkedObject())
        {
            delete elem;
        }
//        if(tab->childrenPointerList().length() == 0)
//            ui->actionRemove_server->setEnabled(false);
    }
}

void Tab_Room::restructWidget()
{
    clear();
    for (const auto &[serverName, deviceNameList] : _data)
    {
        QObject *server = serverSearch(serverName);
        if(server != nullptr)
        {
            for (const auto &deviceName : deviceNameList)
                addWidgets(deviceName, server);
        }
    }
}

QObject *Tab_Room::serverSearch(const QString &serverName) const
{
    QObject *server = nullptr;
    for (auto obj : _serverTab.childrenPointerList())
    {
        if(obj->objectName() == serverName)
        {
            server = obj;
            break;
        }
    }
    return server;
}

void Tab_Room::addWidgets(const QString &deviceName, QObject *server)
{
    for (auto [name, dev] : qobject_cast<GUI_Server*>(server)->getDevices())
    {
        if(deviceName == name)
        {
            GUI_Base_Device* d = GUI_Device_Creator::createGUIDevice(*dev);
            d->setEnabled(false);
            addWidget(d);
            break;
        }
    }
}
