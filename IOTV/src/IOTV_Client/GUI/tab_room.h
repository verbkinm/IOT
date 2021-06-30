#ifndef TAB_ROOM_H
#define TAB_ROOM_H

#include "tab.h"
#include "server.h"
#include <map>

class Tab_Room : public Tab
{
    Q_OBJECT
public:
    Tab_Room(const Tab &serverTab, QWidget *parent);

    void addDevice(const QString &serverName, const QString &deviceName);

    void restructWidget();

private:
    std::map<QString, std::set<QString>> _data; //server name, devices name list
    const Tab &_serverTab;

};

#endif // TAB_ROOM_H
