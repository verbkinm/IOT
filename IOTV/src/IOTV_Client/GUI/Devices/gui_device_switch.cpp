#include "gui_device_switch.h"
#include "device.h"

GUI_Device_Switch::GUI_Device_Switch(Device &device, QWidget *parent) : GUI_Base_Device(device, parent)
{
    _button.setFixedSize(64, 64);
    _button.setIcon(QIcon(":/devices/switch_off"));
    _button.setIconSize(QSize(62, 62));

    _main_layout.addWidget(&_button, 2, 0, 1, 3, Qt::AlignCenter);

    connect(&_button, &QPushButton::clicked, this, &GUI_Device_Switch::slotButtonPressed);

    _device.setAutoReadInterval(1000);
    _device.autoReadEnable(true);
}

void GUI_Device_Switch::update()
{
    Raw::RAW raw = _device.getReadChannelData(0);
    if(raw.ui8 == 0 && (_button.icon().name() != ":/devices/switch_off"))
        _button.setIcon(QIcon(":/devices/switch_off"));
    else if(raw.ui8 == 1 && (_button.icon().name() != ":/devices/switch_on"))
        _button.setIcon(QIcon(":/devices/switch_on"));

    if(_device.getState() && !isEnabled())
        setEnabled(true);
    else if(!_device.getState() && isEnabled())
        setEnabled(false);

    if(_device.getViewName() != _viewName.text())
        setViewName(_device.getViewName());
}

void GUI_Device_Switch::slotButtonPressed()
{
    Raw::RAW raw = _device.getReadChannelData(0);
    if(raw.ui8 == 0)
    {
        raw.ui8 = 1;
        _button.setIcon(QIcon(":/devices/switch_on"));
    }
    else if(raw.ui8 == 1)
    {
        raw.ui8 = 0;
        _button.setIcon(QIcon(":/devices/switch_on"));
    }

    _device.writeData(0, raw);
//    QThread::msleep(10);
//    _device.readData(0);
}
