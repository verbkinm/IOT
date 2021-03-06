#ifndef TAB_ROOM_H
#define TAB_ROOM_H

#include <map>
#include <set>

#include "tab.h"
#include "GUI/gui_server.h"
#include "GUI/Devices/gui_device_creator.h"
#include "GUI/Devices/gui_device_cap.h"
#include "objectlist.h"

class Tab_Room : public Tab
{
    Q_OBJECT
public:
    Tab_Room(const Tab &serverTab, QWidget *parent);

    void addDevice(const QString &serverName, const QString &deviceName);
    void deleteDevices();

    void restructWidget();

    const std::map<QString, std::set<QString>> &data() const;

private:
    QObject *serverSearch(const QString &serverName) const;
    void addWidgets(const QString &deviceName, QObject *server);

    std::map<QString, std::set<QString>> _data; //server name, devices name list
    const Tab &_serverTab;

};

#endif // TAB_ROOM_H
