#include "gui_device_weather_station.h"
#include "device.h"

GUI_Device_Weather_Station::GUI_Device_Weather_Station(Device &device, QWidget *parent) : GUI_Base_Device(device, parent)
{
    _l1.setText("Temperature (C): ");
    _l2.setText("Humidity (%): ");
    _l3.setText("Pressure (mm Hg): ");

    _main_layout.addWidget(&_l1, 2, 0);
    _main_layout.addWidget(&_l2, 3, 0);
    _main_layout.addWidget(&_l3, 4, 0);

    _le1.setReadOnly(true);
    _le2.setReadOnly(true);
    _le3.setReadOnly(true);

    _le1.setAlignment(Qt::AlignCenter);
    _le2.setAlignment(Qt::AlignCenter);
    _le3.setAlignment(Qt::AlignCenter);

    _main_layout.addWidget(&_le1, 2, 1, 1, 2);
    _main_layout.addWidget(&_le2, 3, 1, 1, 2);
    _main_layout.addWidget(&_le3, 4, 1, 1, 2);

    _device.autoReadEnable(true);
}

void GUI_Device_Weather_Station::update()
{
    _le1.setText(Raw::toString(_device.getReadChannelDataType(0), _device.getReadChannelData(0)).c_str());
    _le2.setText(Raw::toString(_device.getReadChannelDataType(1), _device.getReadChannelData(1)).c_str());
    _le3.setText(Raw::toString(_device.getReadChannelDataType(2), _device.getReadChannelData(2)).c_str());

    if(_device.getState())
        setEnabled(true);
    else
        setEnabled(false);

    setViewName(_device.getViewName());
}
