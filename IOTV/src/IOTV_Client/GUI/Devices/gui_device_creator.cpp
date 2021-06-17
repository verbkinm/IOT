#include "gui_device_creator.h"

GUI_Base_Device *GUI_Device_Creator::createGUIDevice(Device &device)
{
    switch (device.getId())
    {
    case 0:
        return new GUI_Device_Unknow(device);
        break;
    case 1:
        return new GUI_Device_Switch(device);
    case 2:
        return new GUI_Device_Weather_Station(device);
    }

    return new GUI_Device_Unknow(device);
}
