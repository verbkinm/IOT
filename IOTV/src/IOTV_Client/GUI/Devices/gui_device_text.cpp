#include "gui_device_text.h"

GUI_Device_Text::GUI_Device_Text(Device &device, QWidget *parent) : GUI_Base_Device(device, parent)
{
    _text.setReadOnly(true);
    _editText.setText("edit");

    _main_layout.addWidget(&_text,  2, 0, 1, 3, Qt::AlignCenter);
//    _main_layout.addWidget(&_editText,  3, 0, 1, 1, Qt::AlignCenter);

    _device.setAutoReadInterval(2000);
    _device.setAutoReadEnable(true);
}

void GUI_Device_Text::update()
{
    Raw::RAW raw = _device.getReadChannelData(0);

    if(raw.str == nullptr)
        _text.clear();
    else if(strcmp(raw.str, _text.toPlainText().toStdString().c_str()) != 0)
    {
        _text.clear();
        _text.setPlainText(raw.str);
    }

    stateAndViewName();
}

void GUI_Device_Text::slotEditText()
{

}

void GUI_Device_Text::slotWrite()
{

}

void GUI_Device_Text::slotShowFull()
{
    _full.reset();
    _full = std::make_unique<Gui_Full_Text>(_device);
    _full->show();
}
