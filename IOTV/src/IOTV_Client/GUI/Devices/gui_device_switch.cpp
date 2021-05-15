#include "gui_device_switch.h"
#include "device.h"

GUI_Device_Switch::GUI_Device_Switch(std::shared_ptr<Device> device, QWidget *parent) : GUI_Base_Device(device, parent)
{
    _button.setFixedSize(86, 86);
    _button.setIcon(QIcon(":/devices/switch_on"));
    _button.setIconSize(QSize(82, 82));

    _main_layout.addWidget(&_button, 1, 0, 1, 2, Qt::AlignCenter);

    connect(&_button, &QPushButton::clicked, this, &GUI_Device_Switch::slotButtonPressed);
}

void GUI_Device_Switch::update()
{

}

void GUI_Device_Switch::slotButtonPressed()
{
    Raw::RAW raw;
    raw.ui64 = 0;
    _device->writeData(0, raw);
    _device->readData(0);
}
