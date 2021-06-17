#ifndef GUI_DEVICE_CREATOR_H
#define GUI_DEVICE_CREATOR_H

#include "gui_device_switch.h"
#include "gui_device_weather_station.h"
#include "gui_device_unknow.h"

#include "device.h"

#include <memory>

class GUI_Device_Creator
{
public:    
    static GUI_Base_Device* createGUIDevice(Device &device);
};

#endif // GUI_DEVICE_CREATOR_H

