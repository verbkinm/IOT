#include "devicelist.h"

DeviceList::DeviceList(std::map<QString, std::shared_ptr<Device> > &devices, QWidget *parent) :
    Tab(parent),
    _devices(devices)
{
    hideButtons();
}

void DeviceList::restructWidget()
{
    for (auto &elem : _devices)
    {
        GUI_Base_Device* d = GUI_Device_Creator::createGUIDevice(*elem.second);
        d->setEnabled(false);
        addWidget(d);
    }
}
