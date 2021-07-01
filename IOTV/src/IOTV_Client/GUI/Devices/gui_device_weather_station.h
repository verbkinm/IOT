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
    QLabel _labelTemperature, _labelHumidity, _labelPressure;
    QLineEdit _temperature, _humidity, _pressure;
};

#endif // GUI_DEVICE_WEATHER_STATION_H
