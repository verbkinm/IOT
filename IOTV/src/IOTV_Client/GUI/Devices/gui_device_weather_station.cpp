#include "gui_device_weather_station.h"
#include "device.h"

#include <math.h>

GUI_Device_Weather_Station::GUI_Device_Weather_Station(Device &device, QWidget *parent) : GUI_Base_Device(device, parent)
{
    _labelTemperature.setText("Temperature (C): ");
    _labelHumidity.setText("Humidity (%): ");
    _labelPressure.setText("Pressure (mm Hg): ");

    _main_layout.addWidget(&_labelTemperature, 2, 0);
    _main_layout.addWidget(&_labelHumidity, 3, 0);
    _main_layout.addWidget(&_labelPressure, 4, 0);

    _temperature.setReadOnly(true);
    _humidity.setReadOnly(true);
    _pressure.setReadOnly(true);

    _temperature.setAlignment(Qt::AlignCenter);
    _humidity.setAlignment(Qt::AlignCenter);
    _pressure.setAlignment(Qt::AlignCenter);

    _main_layout.addWidget(&_temperature, 2, 1, 1, 2);
    _main_layout.addWidget(&_humidity, 3, 1, 1, 2);
    _main_layout.addWidget(&_pressure, 4, 1, 1, 2);

    if(_device.readChannelLength() == 2)
    {
        _labelPressure.hide();
        _pressure.hide();
    }

    _device.setAutoReadEnable(true);

    _showFull.setDisabled(true);
}

void GUI_Device_Weather_Station::update()
{
    //    QString temperature = Raw::toString(_device.getReadChannelDataType(0), _device.getReadChannelData(0)).c_str();
    //    QString humidity = Raw::toString(_device.getReadChannelDataType(1), _device.getReadChannelData(1)).c_str();
    //    QString pressure = Raw::toString(_device.getReadChannelDataType(2), _device.getReadChannelData(2)).c_str();

    //    if(_temperature.text() != temperature)
    //        _temperature.setText(temperature);
    //    if(_humidity.text() != humidity)
    //        _humidity.setText(humidity);
    //    if(_pressure.text() != pressure)
    //        _pressure.setText(pressure);

    float t = _device.getReadChannelData(0).f;
    float h = _device.getReadChannelData(1).f;

    _temperature.setText(QString::number(round(t * 100) / 100));
    _humidity.setText(QString::number(round(h * 100) / 100));

    if(_device.readChannelLength() == 3)
    {
        float p = _device.getReadChannelData(2).f;
        _pressure.setText(QString::number(round(p * 100) / 100));
    }

    stateAndViewName();
}
