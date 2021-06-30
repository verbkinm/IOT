#include "tab_room.h"

Tab_Room::Tab_Room(const Tab &serverTab, QWidget *parent) : Tab(parent), _serverTab(serverTab)
{

}

void Tab_Room::addDevice(const QString &serverName, const QString &deviceName)
{
    _data[serverName].insert(deviceName);
}

void Tab_Room::restructWidget()
{
    clear();
    for (const auto &[serverName, deviceNameList] : _data)
    {
        QObject *server = nullptr;
        for (auto it : _serverTab.childrenPointerList())
        {
            GUI_Server *gui_server = qobject_cast<GUI_Server*>(it);
            if(gui_server->server().objectName() == serverName)
            {
                server = it;
                break;
            }
        }
        if(server != nullptr)
        {
            for (const auto &deviceName : deviceNameList)
            {
                auto _devices = qobject_cast<GUI_Server*>(server)->server().getDevices();
                for (auto [name, dev] : _devices)
                {
                    if(deviceName == name)
                    {
                        GUI_Base_Device* d = GUI_Device_Creator::createGUIDevice(*dev);
                        d->setEnabled(false);
                        addWidget(d);
                        break;
                    }
                    else
                        qDebug() << "not found device " << deviceName << " on server" << serverName;
                }
            }
        }
        else
            qDebug() << "not found server " << serverName;
    }
}
