#ifndef DEVICELIST_H
#define DEVICELIST_H

#include "GUI/tab.h"
#include <GUI/Devices/gui_device_creator.h>

class DeviceList : public Tab
{
    Q_OBJECT

public:
    DeviceList(std::map<QString, std::shared_ptr<Device>> &devices, QWidget *parent = nullptr);
    void restructWidget();

private:
    std::map<QString, std::shared_ptr<Device>> &_devices;



};

#endif // DEVICELIST_H
