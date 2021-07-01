#include "gui_device_switch.h"

static QString fileNameOn = ":/devices/switch_on";
static QString fileNameOff = ":/devices/switch_off";

GUI_Device_Switch::GUI_Device_Switch(Device &device, QWidget *parent) : GUI_Base_Device(device, parent),
    _buttonState(false)
{
    _button.setFixedSize(64, 64);
    _button.setIcon(QIcon(fileNameOff));
    _button.setIconSize(QSize(62, 62));

    _main_layout.addWidget(&_button, 2, 0, 1, 3, Qt::AlignCenter);

    connect(&_button, &QPushButton::clicked, this, &GUI_Device_Switch::slotButtonPressed);

    _device.setAutoReadInterval(1000);
    _device.setAutoReadEnable(true);
}

void GUI_Device_Switch::update()
{
    Raw::RAW raw = _device.getReadChannelData(0);
    if(raw.ui8 == 0 && _buttonState)
    {
        _button.setIcon(QIcon(fileNameOff));
        _buttonState = false;
    }
    else if(raw.ui8 == 1 && !_buttonState)
    {
        _button.setIcon(QIcon(fileNameOn));
        _buttonState = true;
    }

    stateAndViewName();
}

void GUI_Device_Switch::slotButtonPressed()
{
    Raw::RAW raw = _device.getReadChannelData(0);
    if(raw.ui8 == 0)
    {
        raw.ui8 = 1;
        _button.setIcon(QIcon(fileNameOn));
    }
    else if(raw.ui8 == 1)
    {
        raw.ui8 = 0;
        _button.setIcon(QIcon(fileNameOff));
    }

    _device.writeData(0, raw);
}
