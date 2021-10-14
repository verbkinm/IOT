#include "tab_room.h"

Tab_Room::Tab_Room(const Tab &serverTab, QWidget *parent) : Tab(parent), _serverTab(serverTab)
{

}

void Tab_Room::addDevice(const QString &serverName, const QString &deviceName)
{
    _data[serverName].insert(deviceName);
}

void Tab_Room::deleteDevices()
{
    ObjectList objList(childrenPointerList());

    if(objList.exec() == QDialog::Accepted)
    {
        for(auto &elem : objList.checkedObject())
        {
            if(elem->metaObject()->className() == QString("GUI_Base_Device"))
            {
                GUI_Base_Device *gui_base_device = qobject_cast<GUI_Base_Device*>(elem);
                QString server = gui_base_device->getServerObjectName();

                _data[server].erase(gui_base_device->getName());
                if(_data[server].size() == 0)
                    _data.erase(server);
            }
            else if(elem->metaObject()->className() == QString("GUI_Device_Cap"))
            {
                GUI_Device_Cap *gui_device_cap = qobject_cast<GUI_Device_Cap*>(elem);
                QString server = gui_device_cap->getServerName();

                _data[server].erase(gui_device_cap->getName());
                if(_data[server].size() == 0)
                    _data.erase(server);
            }
            delete elem;
        }
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
        else
        {
            for (auto &deviceName : deviceNameList)
            {
                GUI_Device_Cap *cap = new GUI_Device_Cap(serverName, deviceName);
                cap->setObjectName(deviceName);
                addWidget(cap);
            }
        }
    }
}

const std::map<QString, std::set<QString> > &Tab_Room::data() const
{
    return _data;
}

QObject *Tab_Room::serverSearch(const QString &serverName) const
{
    QObject *server = nullptr;
    foreach(const auto &obj, _serverTab.childrenPointerList())
    {
        if( (obj->objectName() == serverName) && (qobject_cast<GUI_Server*>(obj)->state() == QAbstractSocket::ConnectedState) )
        {
            server = obj;
            break;
        }
    }
    return server;
}

void Tab_Room::addWidgets(const QString &deviceName, QObject *server)
{
    for (const auto &[name, dev] : qobject_cast<GUI_Server*>(server)->getDevices())
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
