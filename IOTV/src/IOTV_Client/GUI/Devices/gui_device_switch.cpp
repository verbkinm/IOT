#include "gui_device_switch.h"
#include "device.h"

GUI_Device_Switch::GUI_Device_Switch(Device &device, QWidget *parent) : GUI_Base_Device(device, parent)
{
    _button.setFixedSize(64, 64);
    _button.setIcon(QIcon(":/devices/switch_off"));
    _button.setIconSize(QSize(62, 62));

    _main_layout.addWidget(&_button, 1, 0, 1, 2, Qt::AlignCenter);

    connect(&_button, &QPushButton::clicked, this, &GUI_Device_Switch::slotButtonPressed);

    _device.setAutoReadInterval(200);
    _device.autoReadEnable(true);
}

void GUI_Device_Switch::update()
{
    Raw::RAW raw = _device.getReadChannelData(0);
    if(raw.ui8 == 0)
        _button.setIcon(QIcon(":/devices/switch_off"));
    else if(raw.ui8 == 1)
        _button.setIcon(QIcon(":/devices/switch_on"));

    _button.setEnabled(true);

    if(_device.getState())
        setEnabled(true);
    else
        setEnabled(false);
}

void GUI_Device_Switch::slotButtonPressed()
{
    Raw::RAW raw = _device.getReadChannelData(0);
    if(raw.ui8 == 0)
        raw.ui8 = 1;
    else if(raw.ui8 == 1)
        raw.ui8 = 0;

    _device.writeData(0, raw);
    _button.setDisabled(true);
}
