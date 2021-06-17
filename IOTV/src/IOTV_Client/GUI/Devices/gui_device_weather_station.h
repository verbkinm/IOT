#ifndef GUI_DEVICE_WEATHER_STATION_H
#define GUI_DEVICE_WEATHER_STATION_H

#include <QLineEdit>
#include "gui_base_device.h"

class GUI_Device_Weather_Station : public GUI_Base_Device
{
public:
    GUI_Device_Weather_Station(Device &device, QWidget *parent = nullptr);

public:
    virtual void update() override;

private:
    QLabel _l1, _l2, _l3;
    QLineEdit _le1, _le2, _le3;

};

#endif // GUI_DEVICE_WEATHER_STATION_H
